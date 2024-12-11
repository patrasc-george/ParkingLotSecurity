import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, ElementRef, HostListener, OnInit, Renderer2, ViewChild } from '@angular/core';
import { Router } from '@angular/router';
import { Observable } from 'rxjs/internal/Observable';

@Component({
  selector: 'app-subscriptions',
  templateUrl: './subscription.component.html',
  styleUrls: ['./subscription.component.css']
})
export class SubscriptionComponent implements OnInit {
  vehiclesTable: any[] = [];
  selectedRows = new Set<any>();
  dropdownVisible = false;
  tableErrorMessage = '';
  allHistoryExpanded = false;
  pageTitle: string = '';
  name: string = '';

  @ViewChild('tableBody', { static: true }) tableBody!: ElementRef;

  constructor(private router: Router, private http: HttpClient, private renderer: Renderer2) { }

  ngOnInit(): void {
    this.name = localStorage.getItem('name') || 'Guest';
    const subscriptionName = localStorage.getItem('subscriptionName');
    if (subscriptionName) {
      this.pageTitle = subscriptionName;
    }

    this.loadVehiclesFromLocalStorage();
  }

  loadVehiclesFromLocalStorage(): void {
    try {
      this.vehiclesTable = JSON.parse(localStorage.getItem('vehiclesTable') || '[]');
    } catch (error) {
      console.error('Error parsing vehiclesTable from localStorage:', error);
    }
  }

  toggleDropdown(): void {
    this.dropdownVisible = !this.dropdownVisible;
  }

  @HostListener('document:click', ['$event'])
  closeDropdown(event: MouseEvent): void {
    const dropdown = document.getElementById('accountDropdown');
    const accountIcon = document.querySelector('.accountIconContainer');

    if (this.dropdownVisible && dropdown && !dropdown.contains(event.target as Node) && !accountIcon?.contains(event.target as Node)) {
      this.dropdownVisible = false;
    }
  }

  navigateTo(destination: string): void {
    const routes: { [key: string]: string } = {
      account: '/account',
      subscriptions: '/subscriptions',
      login: '/login',
      contact: '/contact'
    };

    this.router.navigate([routes[destination]]);
  }

  footerNavigateTo(destination: string): void {
    localStorage.setItem('policy', destination);
    this.router.navigateByUrl('/').then(() => {
      this.router.navigate(['/terms-and-conditions']);
    });
  }

  subscribeNewsletter(email: string): void {
    (document.querySelector('form input') as HTMLInputElement).value = '';

    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
    if (!emailRegex.test(email))
      return;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    this.http.post('http://localhost:8080/api/subscribeNewsletter', urlEncodedData.toString(), { headers })
      .subscribe();
  }

  redirectToSubscriptions(event: Event) {
    event.preventDefault();
    this.router.navigate(['/subscriptions']);
  }

  onRowSelect(event: Event, vehicle: any): void {
    const checkbox = event.target as HTMLInputElement;
    const row = checkbox.closest('tr');
    if (checkbox.checked) {
      if (row) {
        row.classList.add('selected-row');
        this.selectedRows.add(vehicle[0]);
      }
    } else {
      if (row) {
        row.classList.remove('selected-row');
        this.selectedRows.delete(vehicle[0]);
      }
    }
  }

  uncheckAll(): void {
    this.selectedRows.clear();

    const checkboxes = document.querySelectorAll('input[type="checkbox"]');

    checkboxes.forEach(checkbox => {
      const inputCheckbox = checkbox as HTMLInputElement;
      inputCheckbox.checked = false;

      const row = checkbox.closest('tr');
      if (row) {
        row.classList.remove('selected-row');
      }
    });
  }

  checkAll(): void {
    const checkboxes = document.querySelectorAll('input[type="checkbox"]');

    checkboxes.forEach(checkbox => {
      const inputCheckbox = checkbox as HTMLInputElement;
      inputCheckbox.checked = true;

      const row = checkbox.closest('tr');
      if (row) {
        row.classList.add('selected-row');
        const cells = Array.from(row.children) as HTMLTableCellElement[];
        const licensePlate = cells[0].textContent?.trim() || '';

        if (licensePlate) {
          this.selectedRows.add(licensePlate);
        }
      }
    });
  }

  viewAllHistory(): void {
    if (this.vehiclesTable.length === 0) {
      return;
    }

    this.tableErrorMessage = '';

    this.vehiclesTable.forEach(vehicle => {
      const licensePlate = vehicle[0];

      const row = Array.from(this.tableBody.nativeElement.children)
        .find((tr: unknown) => (tr as Element).textContent?.includes(licensePlate)) as HTMLTableRowElement;

      if (row && row.classList.contains('expanded') === this.allHistoryExpanded) {
        this.viewHistory(licensePlate);
      }
    });

    this.allHistoryExpanded = !this.allHistoryExpanded;
    this.uncheckAll();
  }

  viewHistory(licensePlate: string): void {
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('licensePlate', licensePlate);

    this.http.post<{ success: boolean; history: any[] }>(
      'http://localhost:8080/api/getVehicleHistory',
      urlEncodedData.toString(),
      { headers: new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' }) }
    ).subscribe(
      data => {
        if (data.success && Array.isArray(data.history)) {
          const row = Array.from(this.tableBody.nativeElement.children)
            .filter((node): node is HTMLTableRowElement => node instanceof HTMLTableRowElement)
            .find((tr) => tr.textContent?.includes(licensePlate));

          if (row) {
            this.toggleHistoryRows(row, data.history);
          }
        }
      },
      error => {
        console.error('Eroare:', error);
      }
    );
  }

  private toggleHistoryRows(row: HTMLTableRowElement, historyData: any[]): void {
    const isExpanded = row.classList.toggle('expanded');

    if (isExpanded) {
      const reversedHistoryData = [...historyData].reverse();

      const statusCell = row.querySelector('td:last-child');
      const vehicleStatus = statusCell ? statusCell.textContent?.trim() : '';
      if (vehicleStatus === 'Inactive') reversedHistoryData.pop();

      reversedHistoryData.forEach(historyEntry => {
        const newRow = this.renderer.createElement('tr');
        newRow.classList.add('history-row', 'fall-in');

        const numColumns = 8;
        for (let i = 0; i < numColumns; i++) {
          const newCell = this.renderer.createElement('td');

          if (i === 1) newCell.textContent = historyEntry[0] || '';
          else if (i === 2) newCell.textContent = historyEntry[1] || '';
          else if (i === 3) newCell.textContent = historyEntry[2] || '';
          else if (i === 4) newCell.textContent = historyEntry[3] || '';

          this.renderer.appendChild(newRow, newCell);
        }

        this.renderer.insertBefore(row.parentNode, newRow, row.nextSibling);
      });
    } else {
      let nextSibling = row.nextSibling as HTMLElement;
      while (nextSibling && nextSibling.classList.contains('history-row')) {
        nextSibling.remove();
        nextSibling = row.nextSibling as HTMLElement;
      }
    }
  }

  addVehicleRow(): void {
    this.uncheckAll();
    this.tableErrorMessage = '';
    const newRow = this.createEditableRow();
    let isSaved = false;

    const saveChanges = () => {
      if (isSaved) return;
      const { licensePlate, activityData } = this.extractDataFromRow(newRow);

      if (licensePlate) {
        this.saveVehicleToServer(licensePlate, activityData)
          .subscribe(
            (response: any) => this.handleSaveSuccess(response, newRow),
            (error: any) => this.handleSaveError(error, newRow)
          );

      } else {
        this.removeRow(newRow);
      }
      isSaved = true;
    };

    this.addSaveListenersToRow(newRow, saveChanges);

    this.renderer.appendChild(this.tableBody.nativeElement, newRow);
    if (newRow.firstChild) {
      (newRow.firstChild as HTMLElement).focus();
    }
  }

  private createEditableRow(): HTMLTableRowElement {
    const newRow = this.renderer.createElement('tr');
    const numberOfColumns = 8;

    for (let i = 0; i < numberOfColumns; i++) {
      const newCell = this.renderer.createElement('td');
      this.renderer.setAttribute(newCell, 'contenteditable', 'true');
      this.renderer.setProperty(newCell, 'textContent', '');
      this.renderer.appendChild(newRow, newCell);
    }

    return newRow;
  }

  private extractDataFromRow(row: HTMLTableRowElement): { licensePlate: string, activityData: string[] } {
    const cells = Array.from(row.children) as HTMLTableCellElement[];
    const licensePlate = cells[0].textContent?.trim() || '';
    const activityData = cells.slice(1).map(cell => cell.textContent?.trim() || '');
    return { licensePlate, activityData };
  }

  private saveVehicleToServer(licensePlate: string, activityData: string[]): Observable<any> {
    const email = localStorage.getItem('email') || '';
    const subscriptionName = localStorage.getItem('subscriptionName') || '';

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', email);
    urlEncodedData.append('licensePlate', licensePlate);
    urlEncodedData.append('subscriptionName', subscriptionName);
    urlEncodedData.append('activityData', JSON.stringify(activityData));

    return this.http.post('http://localhost:8080/api/addVehicle', urlEncodedData.toString(), {
      headers: new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' })
    });
  }

  private handleSaveSuccess(response: any, row: HTMLTableRowElement): void {
    if (response.success) {
      this.vehiclesTable.push(response.vehiclesTable[0]);
      localStorage.setItem('vehiclesTable', JSON.stringify(this.vehiclesTable));
      this.removeRow(row);
    } else {
      this.tableErrorMessage = response.message || 'The vehicle already exists.';
      this.removeRow(row);
    }
  }

  private handleSaveError(error: any, row: HTMLTableRowElement): void {
    console.error('Error:', error);
    this.tableErrorMessage = 'An error occurred. Please try again later.';
    this.removeRow(row);
  }

  private removeRow(row: HTMLTableRowElement): void {
    if (this.tableBody.nativeElement.contains(row)) {
      this.renderer.removeChild(this.tableBody.nativeElement, row);
    }
  }

  private addSaveListenersToRow(row: HTMLTableRowElement, saveChanges: () => void): void {
    Array.from(row.children).forEach(cell => {
      this.renderer.listen(cell, 'focusout', saveChanges);
      this.renderer.listen(cell, 'keydown', (e: KeyboardEvent) => {
        if (e.key === 'Enter') {
          e.preventDefault();
          saveChanges();
        }
      });
    });
  }

  deleteSelected(): void {
    if (this.allHistoryExpanded) {
      const copySelectedRows = this.selectedRows;
      this.viewAllHistory();
      this.selectedRows = copySelectedRows;
    }
    const email = localStorage.getItem('email');
    const subscriptionName = localStorage.getItem('subscriptionName');

    if (!email || !subscriptionName) {
      return;
    }

    this.selectedRows.forEach(vehicle => {
      const licensePlate = vehicle;
      const urlEncodedData = new URLSearchParams();
      urlEncodedData.append('email', email);
      urlEncodedData.append('subscriptionName', subscriptionName);
      urlEncodedData.append('licensePlate', licensePlate);

      fetch('http://localhost:8080/api/deleteVehicle', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: urlEncodedData.toString()
      })
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            this.vehiclesTable = this.vehiclesTable.filter(v => v[0] !== licensePlate);
            localStorage.setItem('vehiclesTable', JSON.stringify(this.vehiclesTable));
          } else {
            this.tableErrorMessage = 'Failed to delete vehicle.';
          }
        })
        .catch(error => {
          console.error('Error:', error);
          this.tableErrorMessage = 'An error occurred while deleting the vehicle.';
        });
    });

    this.selectedRows.clear();
  }
}
