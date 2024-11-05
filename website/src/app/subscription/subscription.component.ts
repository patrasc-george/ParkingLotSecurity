import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, ElementRef, OnInit, Renderer2, ViewChild } from '@angular/core';
import { Router } from '@angular/router';

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

  @ViewChild('tableBody', { static: true }) tableBody!: ElementRef;

  constructor(private router: Router, private http: HttpClient, private renderer: Renderer2) {}

  ngOnInit(): void {
    this.loadVehiclesFromLocalStorage();
  }

  loadVehiclesFromLocalStorage(): void {
    try {
      this.vehiclesTable = JSON.parse(localStorage.getItem('vehiclesTable') || '[]');
    } catch (error) {
      console.error('Error parsing vehiclesTable from localStorage:', error);
    }
  }

  onRowSelect(event: Event, vehicle: any): void {
    const checkbox = event.target as HTMLInputElement;
    if (checkbox.checked) {
      this.selectedRows.add(vehicle);
    } else {
      this.selectedRows.delete(vehicle);
    }
  }

  toggleDropdown(): void {
    this.dropdownVisible = !this.dropdownVisible;
  }

  navigateTo(destination: string): void {
    const routes: { [key: string]: string } = {
      account: '/account',
      subscriptions: '/subscriptions',
      login: '/login'
    };

    this.router.navigate([routes[destination]]);
  }

  checkAll(): void {
    this.selectedRows = new Set(this.vehiclesTable);
  }

  uncheckAll(): void {
    this.selectedRows.clear();
  }

  viewSelected(): void {
    this.selectedRows.forEach(vehicle => {
      this.viewHistory(vehicle[0]);
    });
  }

  viewHistory(licensePlate: string): void {
    fetch('http://localhost:8080/api/getVehicleHistory', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: new URLSearchParams({ licensePlate })
    })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          // Process and display history
        }
      })
      .catch(error => {
        console.error('Error:', error);
        this.tableErrorMessage = 'Error communicating with the server.';
      });
  }
  addVehicleRow(): void {
    // Deselectează toate vehiculele și elimină clasele selectate
    this.vehiclesTable.forEach(vehicle => vehicle.selected = false);
    this.selectedRows.clear();

    // Creăm un nou rând
    const newRow = this.renderer.createElement('tr');

    // Flag pentru a preveni salvarea multiplă
    let isSaved = false;

    // Funcția de salvare a vehiculului
    const saveChanges = () => {
        // Obținem datele din celule
        const newCells = Array.from(newRow.children) as HTMLTableCellElement[];
        const licensePlate = newCells[0].textContent?.trim() || '';
        const activityData = newCells.slice(1).map(cell => cell.textContent?.trim() || '');

        if (licensePlate) {
            const name = localStorage.getItem('name') || '';
            const subscriptionName = localStorage.getItem('subscriptionName') || '';

            const urlEncodedData = new URLSearchParams();
            urlEncodedData.append('name', name);
            urlEncodedData.append('licensePlate', licensePlate);
            urlEncodedData.append('subscriptionName', subscriptionName);
            urlEncodedData.append('activityData', JSON.stringify(activityData));

            this.http.post('http://localhost:8080/api/addVehicle', urlEncodedData.toString(), {
                headers: new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' })
            })
            .subscribe(
                (data: any) => {
                    if (data.success) {
                        this.vehiclesTable.push(data.vehiclesTable[0]);
                        localStorage.setItem('vehiclesTable', JSON.stringify(this.vehiclesTable));

                        // Aici adăugăm logică pentru a crea un rând nou
                        this.addVehicleRowToTable(data.vehiclesTable[0]);
                    } else {
                        this.tableErrorMessage = data.message || 'The vehicle already exists.';
                        removeNewRow(); // Apelăm funcția de eliminare
                    }
                },
                (error: any) => {
                    console.error('Error:', error);
                    this.tableErrorMessage = 'An error occurred. Please try again later.';
                    removeNewRow(); // Apelăm funcția de eliminare
                }
            );
        } else {
            removeNewRow(); // Apelăm funcția de eliminare
        }
        isSaved = true;
    };

    // Funcția pentru a elimina newRow în siguranță
    const removeNewRow = () => {
        if (this.tableBody.nativeElement.contains(newRow)) {
            this.renderer.removeChild(this.tableBody.nativeElement, newRow);
        }
    };

    // Creăm celule editabile pentru toate coloanele
    const numberOfColumns = 7;
    for (let i = 0; i < numberOfColumns; i++) {
        const newCell = this.renderer.createElement('td');
        this.renderer.setAttribute(newCell, 'contenteditable', 'true');
        this.renderer.setProperty(newCell, 'textContent', '');

        // Adăugăm event listener pentru fiecare celulă
        this.renderer.listen(newCell, 'focusout', () => {
            if (!isSaved) saveChanges();
        });

        this.renderer.listen(newCell, 'keydown', (e: KeyboardEvent) => {
            if (e.key === 'Enter') {
                e.preventDefault();
                if (!isSaved) saveChanges();
            }
        });

        // Adăugăm celula la rând
        this.renderer.appendChild(newRow, newCell);
    }

    // Adăugăm rândul în tabel
    this.renderer.appendChild(this.tableBody.nativeElement, newRow);
    newRow.firstChild.focus(); // Focus pe prima celulă
}

// Funcția pentru a adăuga un vehicul în tabel
private addVehicleRowToTable(vehicle: any): void {
    const row = this.renderer.createElement('tr');

    // Creăm prima celulă cu checkbox
    const firstCell = this.renderer.createElement('td');
    const wrapper = this.renderer.createElement('div');
    this.renderer.addClass(wrapper, 'cell-wrapper');

    const checkboxTextWrapper = this.renderer.createElement('div');
    this.renderer.addClass(checkboxTextWrapper, 'checkbox-text-wrapper');

    const checkbox = this.renderer.createElement('input');
    this.renderer.setAttribute(checkbox, 'type', 'checkbox');

    // Event listener pentru checkbox
    this.renderer.listen(checkbox, 'change', () => {
        if (checkbox.checked) {
            this.renderer.addClass(row, 'selected-row');
            this.selectedRows.add(row);
        } else {
            this.renderer.removeClass(row, 'selected-row');
            this.selectedRows.delete(row);
        }
    });

    // Adăugăm numărul de înmatriculare
    const licensePlateText = this.renderer.createElement('span');
    this.renderer.setProperty(licensePlateText, 'textContent', vehicle[0]);
    this.renderer.appendChild(checkboxTextWrapper, checkbox);
    this.renderer.appendChild(checkboxTextWrapper, licensePlateText);

    // Butonul de vizualizare
    const viewButton = this.renderer.createElement('button');
    this.renderer.setAttribute(viewButton, 'title', "View history");

    const viewButtonIcon = this.renderer.createElement('img');
    this.renderer.setAttribute(viewButtonIcon, 'src', 'data/view_history.png');
    this.renderer.setAttribute(viewButtonIcon, 'alt', 'View');
    this.renderer.addClass(viewButtonIcon, 'icon');
    this.renderer.appendChild(viewButton, viewButtonIcon);
    this.renderer.addClass(viewButton, 'view-button');

    this.renderer.appendChild(wrapper, checkboxTextWrapper);
    this.renderer.appendChild(wrapper, viewButton);
    this.renderer.appendChild(firstCell, wrapper);
    this.renderer.appendChild(row, firstCell);

    // Adăugăm celulele pentru restul datelor
    const numColumns = 8;
    for (let i = 1; i < numColumns; i++) {
        const cell = this.renderer.createElement('td');
        this.renderer.setProperty(cell, 'textContent', vehicle[i] || '');
        if (i === numColumns - 1) {
            this.renderer.setStyle(cell, 'color', vehicle[i]?.toLowerCase() === 'active' ? 'green' : 'red');
        }
        this.renderer.appendChild(row, cell);
    }

    // this.vehiclesTable.push(row);

    // Adăugăm rândul în tabel
    this.renderer.appendChild(this.vehiclesTable, row);
}


  
  deleteSelected(): void {
    this.selectedRows.forEach(vehicle => {
      const licensePlate = vehicle[0];
      fetch('http://localhost:8080/api/deleteVehicle', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: new URLSearchParams({ licensePlate })
      })
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            this.vehiclesTable = this.vehiclesTable.filter(v => v[0] !== licensePlate);
            localStorage.setItem('vehiclesTable', JSON.stringify(this.vehiclesTable));
          }
        })
        .catch(error => {
          console.error('Error:', error);
          this.tableErrorMessage = 'Failed to delete vehicle.';
        });
    });
    this.selectedRows.clear();
  }
}
