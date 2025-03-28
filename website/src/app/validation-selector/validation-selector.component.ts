import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, HostListener, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { Router } from '@angular/router';
import { AuthService } from '../auth.service';
import { environment } from './environment';

@Component({
  selector: 'app-validation-selector',
  templateUrl: './validation-selector.component.html',
  styleUrls: ['./validation-selector.component.css']
})
export class ValidationSelectorComponent implements OnInit {
  dropdownVisible = false;
  validationForm: FormGroup;
  validationMethod: 'email' | 'sms' | null = null;
  name: string = '';
  email: string = '';
  password: string = '';
  phone: string = '';
  isAuthenticated: boolean = true;
  apiURL: string = environment.API_URL;
  key: string = environment.POSTGRES_PASSWORD;;

  constructor(private router: Router, private authService: AuthService, private http: HttpClient, private fb: FormBuilder) {
    this.validationForm = this.fb.group({
      validationMethod: [null, Validators.required],
    });
  }

  ngOnInit(): void {
    if (localStorage.getItem('fromAccount') == 'true') {
      this.isAuthenticated = true;
    } else if (localStorage.getItem('fromCreateSubscription') == 'true') {
      this.isAuthenticated = false;
    }
    this.name = localStorage.getItem('name') || '';
    this.email = localStorage.getItem('email') || '';
    this.phone = localStorage.getItem('phone') || '';
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
      dashboard: '/dashboard',
      account: '/account',
      subscriptions: '/subscriptions',
      login: '/login',
      mainpage: '/',
      createAccount: '/create-subscription',
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
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = this.apiURL + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  redirectToSubscriptions(event: Event): void {
    event.preventDefault();
    if (localStorage.getItem('fromAccount') == 'true') {
      localStorage.setItem('fromAccount', 'false');
      this.router.navigate(['/subscriptions']);
    } else if (localStorage.getItem('fromCreateSubscription') == 'true') {
      localStorage.setItem('fromCreateSubscription', 'false');
      this.router.navigate(['/']);
    }
  }

  onValidate(method: 'email' | 'sms'): void {
    if (method === 'email') {
      this.validateByEmail();
    } else if (method === 'sms') {
      this.validateBySMS();
    }
  }

  validateByEmail() {
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('name', this.name);
    urlEncodedData.append('email', this.email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    if (localStorage.getItem('fromCreateSubscription') === 'true') {
      localStorage.setItem('fromCreateSubscription', 'false');
      const apiUrl = this.apiURL + '/api/validateViaEmail';

      localStorage.setItem('fromValidationSelector', 'true');
      this.router.navigate(['/login']);

      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data: any) => {

          },
          error => {
            console.error('Error:', error);
          }
        );
    } else if (localStorage.getItem('fromAccount') === 'true') {
      localStorage.setItem('fromAccount', 'false');
      const apiUrl = this.apiURL + '/api/validateUpdateViaEmail';

      localStorage.setItem('fromValidationSelector', 'true');
      this.router.navigate(['/login']);

      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data: any) => {

          },
          error => {
            console.error('Error:', error);
          }
        );
    }
  }

  validateBySMS() {
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('email', this.email);
    urlEncodedData.append('phone', this.phone);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    if (localStorage.getItem('fromCreateSubscription') === 'true') {
      localStorage.setItem('fromCreateSubscription', 'false');
      const apiUrl = this.apiURL + '/api/validateViaSMS';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data: any) => {
            localStorage.setItem('fromValidationSelector', 'true');
            this.router.navigate(['/validate-phone']);
          },
          error => {
            console.error('Error:', error);
          }
        );
    } else if (localStorage.getItem('fromAccount') === 'true') {
      const apiUrl = this.apiURL + '/api/validateUpdateViaSMS';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data: any) => {
            this.router.navigate(['/validate-phone']);
          },
          error => {
            console.error('Error:', error);
          }
        );
    }
  }

}
