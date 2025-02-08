import { Component, HostListener, OnInit } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router, ActivatedRoute } from '@angular/router';
import { AuthService } from '../auth.service';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {
  input: string = '';
  password: string = '';
  successMessage: string = '';
  errorMessage: string = '';
  dropdownVisible = false;

  constructor(
    private http: HttpClient,
    private router: Router,
    private route: ActivatedRoute,
    private authService: AuthService
  ) { }

  ngOnInit(): void {
    this.authService.logout();

    this.route.queryParams.subscribe(params => {
      if (localStorage.getItem('fromValidationSelector') === 'true') {
        this.successMessage = 'A validation email has been sent to your address.';
        localStorage.setItem('fromValidationSelector', 'false');
      }
      if (localStorage.getItem('fromRecoverPassword') === 'true') {
        this.successMessage = 'A password recovery email has been sent to your address.';
        localStorage.setItem('fromRecoverPassword', 'false');
      }
      if (localStorage.getItem('invalidLink') === 'true') {
        this.errorMessage = 'The link is invalid.';
        localStorage.setItem('invalidLink', 'false');
      }
    });
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
      mainpage: '/',
      login: '/login',
      createAccount: '/create-subscription',
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

  onLogin() {
    this.successMessage = '';
    this.errorMessage = '';

    if (!this.input || !this.password) {
      this.errorMessage = 'Please enter both email and password.';
      return;
    }

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
    urlEncodedData.append('input', this.input);
    urlEncodedData.append('password', this.password);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/login';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        data => {
          this.handleServerResponse(data);
        },
        error => {
          console.error('Error:', error);
        }
      );
  }

  handleServerResponse(data: any) {
    this.successMessage = '';
    this.errorMessage = '';

    if (data.success === false) {
      this.errorMessage = 'Login failed. Please check your credentials.';
    } else if (data.success === true) {
      if (this.input === "admin") {
        this.router.navigate(['/dashboard']);
      } else {
        localStorage.setItem('subscriptionsTable', JSON.stringify(data.subscriptionsTable));
        localStorage.setItem('name', data.name);
        localStorage.setItem('lastName', data.lastName);
        localStorage.setItem('email', data.email);
        localStorage.setItem('phone', data.phone);
        this.router.navigate(['/subscriptions']);
      }
    } else {
      this.errorMessage = 'An unexpected response was received. Please try again.';
    }
  }
}
