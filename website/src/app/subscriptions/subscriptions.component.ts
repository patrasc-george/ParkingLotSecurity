import { Component, OnInit, Renderer2, ElementRef, ViewChild, HostListener } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';

@Component({
  selector: 'app-subscriptions',
  templateUrl: './subscriptions.component.html',
  styleUrls: ['./subscriptions.component.css']
})
export class SubscriptionsComponent implements OnInit {
  subscriptionsTable: string[] = [];
  selectedRows: Set<string> = new Set();
  tableErrorMessage: string = '';
  dropdownVisible: boolean = false;
  name: string = '';
  isAdmin: boolean = false;

  @ViewChild('tableBody', { static: true }) tableBody!: ElementRef;

  constructor(private http: HttpClient, private router: Router, private renderer: Renderer2) { }

  ngOnInit(): void {
    this.isAdmin = localStorage.getItem('admin') === 'true';
    this.name = localStorage.getItem('name') || 'Guest';
    this.loadSubscriptions();
  }

  loadSubscriptions() {
    try {
      this.subscriptionsTable = JSON.parse(localStorage.getItem('subscriptionsTable') || '[]');
    } catch (error) {
      console.error('Error parsing subscriptionsTable from localStorage:', error);
      this.subscriptionsTable = [];
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

  navigateTo(destination: string) {
    const routes: { [key: string]: string } = {
      dashboard: '/dashboard',
      account: '/account',
      subscriptions: '/subscriptions',
      login: '/login',
      contact: '/contact'
    };

    const route = routes[destination];

    if (this.router.url === route) {
      this.router.navigateByUrl('/').then(() => {
        this.router.navigate([route]);
      });
    } else {
      this.router.navigate([route]);
    }
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
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  redirectToSubscriptions(event: Event) {
    event.preventDefault();
    this.router.navigate(['/subscriptions']);
  }

  logout() {
    this.router.navigate(['/login']);
  }

  toggleSelection(subscription: string) {
    const row = document.querySelector(`tr[data-subscription="${subscription}"]`) as HTMLTableRowElement;

    if (this.selectedRows.has(subscription)) {
      this.selectedRows.delete(subscription);
      row?.classList.remove('selected-row');
    } else {
      this.selectedRows.add(subscription);
      row?.classList.add('selected-row');
    }
  }

  uncheckAll() {
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

  checkAll() {
    const checkboxes = document.querySelectorAll('input[type="checkbox"]');

    checkboxes.forEach(checkbox => {
      const inputCheckbox = checkbox as HTMLInputElement;
      inputCheckbox.checked = true;
      const row = checkbox.closest('tr');

      if (row) {
        row.classList.add('selected-row');
        const subscription = row.getAttribute('data-subscription');

        if (subscription) {
          this.selectedRows.add(subscription);
        }
      }
    });
  }

  addSubscription() {
    this.uncheckAll();

    this.tableErrorMessage = '';
    const newRow = this.renderer.createElement('tr');
    const newSubscriptionCell = this.renderer.createElement('td');
    this.renderer.setAttribute(newSubscriptionCell, 'contenteditable', 'true');
    this.renderer.setProperty(newSubscriptionCell, 'textContent', '');

    let isSaved = false;

    const saveSubscription = () => {
      const subscriptionName = newSubscriptionCell.textContent.trim();
      if (subscriptionName) {
        const email = localStorage.getItem('email');
        const urlEncodedData = new URLSearchParams();
        urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
        urlEncodedData.append('email', email || '');
        urlEncodedData.append('subscriptionName', subscriptionName);

        const apiUrl = window['env'].API_URL + '/api/addSubscription';
        this.http.post(apiUrl, urlEncodedData.toString(), {
          headers: new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' })
        })
          .subscribe(
            (data: any) => {
              if (data.success) {
                this.subscriptionsTable.push(subscriptionName);
                localStorage.setItem('subscriptionsTable', JSON.stringify(this.subscriptionsTable));
                this.renderer.removeChild(this.tableBody.nativeElement, newRow);
              } else {
                this.tableErrorMessage = 'The subscription already exists.';
                this.renderer.removeChild(this.tableBody.nativeElement, newRow);
              }
            },
            error => {
              console.error('Error:', error);
              this.tableErrorMessage = 'An error occurred. Please try again later.';
              this.renderer.removeChild(this.tableBody.nativeElement, newRow);
            }
          );
      } else {
        this.renderer.removeChild(this.tableBody.nativeElement, newRow);
      }
      isSaved = true;
    };

    newSubscriptionCell.addEventListener('focusout', () => {
      if (!isSaved) saveSubscription();
    });

    newSubscriptionCell.addEventListener('keypress', (e: KeyboardEvent) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        if (!isSaved) saveSubscription();
      }
    });

    this.renderer.appendChild(newRow, newSubscriptionCell);
    this.renderer.appendChild(this.tableBody.nativeElement, newRow);
    newSubscriptionCell.focus();
  }

  async deleteSelected() {
    this.tableErrorMessage = '';

    for (const subscription of this.selectedRows) {
      const email = localStorage.getItem('email');
      const urlEncodedData = new URLSearchParams();
      urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
      urlEncodedData.append('email', email || '');
      urlEncodedData.append('subscriptionName', subscription);

      const apiUrl = window['env'].API_URL + '/api/deleteSubscription';
      try {
        const data: any = await this.http.post(apiUrl, urlEncodedData.toString(), {
          headers: new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' })
        }).toPromise();

        if (data.success) {
          this.subscriptionsTable = this.subscriptionsTable.filter(sub => sub !== subscription);
          localStorage.setItem('subscriptionsTable', JSON.stringify(this.subscriptionsTable));
        } else {
          this.tableErrorMessage = `Failed to delete subscription: ${subscription}.`;
          break;
        }
      } catch (error) {
        console.error('Error:', error);
        this.tableErrorMessage = `An error occurred while deleting subscription: ${subscription}. Please try again later.`;
        break;
      }
    }

    this.selectedRows.clear();
  }


  viewSubscription(subscription: string) {
    this.tableErrorMessage = '';

    const email = localStorage.getItem('email');
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
    urlEncodedData.append('email', email || '');
    urlEncodedData.append('subscriptionName', subscription);

    const apiUrl = window['env'].API_URL + '/api/getSubscriptionVehicles';
    this.http.post(apiUrl, urlEncodedData.toString(), {
      headers: new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' })
    })
      .subscribe(
        (data: any) => {
          if (data.success && Array.isArray(data.vehiclesTable)) {
            localStorage.setItem('vehiclesTable', JSON.stringify(data.vehiclesTable));
            localStorage.setItem('subscriptionName', subscription);
            this.router.navigate(['/subscription']);
          } else {
            this.tableErrorMessage = 'Error fetching vehicles for subscription.';
          }
        },
        error => {
          console.error('Error:', error);
          this.tableErrorMessage = 'Error communicating with the server.';
        }
      );
  }
}
