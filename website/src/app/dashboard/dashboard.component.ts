import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, HostListener, OnInit } from '@angular/core';
import { Router } from '@angular/router';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {
  // occupancyTable: any[][] = [];
  // entranceTable: any[][] = [];
  // exitTable: any[][] = [];

  // hours: number[] = Array.from({ length: 24 }, (_, i) => i);
  // daysOfWeek: string[] = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'];

  // occupancyStatistics: number[][] = [];
  // entranceStatistics: number[][] = [];
  // exitStatistics: number[][] = [];

  // normalizedOccupancyStatistics: number[][] = [];
  // normalizedEntranceStatistics: number[][] = [];
  // normalizedExitStatistics: number[][] = [];

  // selectedStatistics: number[][] = [];

  emailsTable: any[] = [];

  dropdownVisible = false;

  constructor(private http: HttpClient, private router: Router) { }

  ngOnInit(): void {
    this.getTables();
  }

  getTables(): void {
    const urlEncodedData = new URLSearchParams();
    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/getAdmin';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => {
          if (data && data.emailsTable) {
            this.emailsTable = data.emailsTable;

            localStorage.setItem('emailsTable', JSON.stringify(data.emailsTable));
          }
          // if (data && data.occupancyTable && data.entranceTable && data.exitTable) {
          //   this.occupancyTable = data.occupancyTable;
          //   this.entranceTable = data.entranceTable;
          //   this.exitTable = data.exitTable;

          //   localStorage.setItem('occupancyTable', JSON.stringify(data.occupancyTable));
          //   localStorage.setItem('entranceTable', JSON.stringify(data.entranceTable));
          //   localStorage.setItem('exitTable', JSON.stringify(data.exitTable));

          //   this.normalizedOccupancyStatistics = this.normalize(this.occupancyTable);
          //   this.normalizedEntranceStatistics = this.normalize(this.entranceTable);
          //   this.normalizedExitStatistics = this.normalize(this.exitTable);

          //   this.selectedStatistics = this.normalizedOccupancyStatistics;
          // } 
          else {
            console.error('Unexpected response format:', data);
          }
        },
        error => {
          console.error('Error:', error);
        }
      );
  }

  // normalize(data: number[][]): number[][] {
  //   let maxValue = 0;

  //   for (const row of data) {
  //     for (const value of row) {
  //       if (value > maxValue) {
  //         maxValue = value;
  //       }
  //     }
  //   }

  //   if (maxValue === 0) return data.map(row => row.map(() => 0));

  //   return data.map(row => row.map(value => value / maxValue));
  // }

  // getColor(value: number): string {
  //   const blueValue = Math.round(255 * value);
  //   return `rgb(${255 - blueValue}, ${255 - blueValue}, 255)`;
  // }

  toggleDropdown() {
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
      login: '/login'
    };

    const route = routes[destination];
    if (route === "/dashboard") {
      window.location.reload();
    } else {
      this.router.navigate([route]);
    }
  }

  // onTableChange(event: Event): void {
  //   const selectedTable = (event.target as HTMLSelectElement).value;

  //   switch (selectedTable) {
  //     case 'occupancy':
  //       this.selectedStatistics = this.normalizedOccupancyStatistics;
  //       break;
  //     case 'entrance':
  //       this.selectedStatistics = this.normalizedEntranceStatistics;
  //       break;
  //     case 'exit':
  //       this.selectedStatistics = this.normalizedExitStatistics;
  //       break;
  //     default:
  //       this.selectedStatistics = [];
  //       break;
  //   }
  // }

  viewAccount(email: string) {
    const urlEncodedData = new URLSearchParams();

    urlEncodedData.append('input', email);
    urlEncodedData.append('fromRedirect', 'true');

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/login';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => {
          if (data.success === true) {
            localStorage.setItem('admin', 'true');
            localStorage.setItem('subscriptionsTable', JSON.stringify(data.subscriptionsTable));
            localStorage.setItem('name', data.name);
            localStorage.setItem('lastName', data.lastName);
            localStorage.setItem('email', data.email);
            localStorage.setItem('phone', data.phone);
            this.router.navigate(['/subscriptions']);
          }
        },
        error => {
          console.error('Error:', error);
        }
      );
  }
}
