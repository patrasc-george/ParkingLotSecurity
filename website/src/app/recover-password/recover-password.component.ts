import { Component, HostListener, OnInit } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';
import { FormGroup, FormBuilder, Validators, AbstractControl, ValidationErrors } from '@angular/forms';

@Component({
  selector: 'app-recover-password',
  templateUrl: './recover-password.component.html',
  styleUrls: ['./recover-password.component.css']
})
export class RecoverPasswordComponent implements OnInit {
  recoverForm: FormGroup;
  errorMessage: string | null = null;
  dropdownVisible = false;
  fieldLabel: string = '';
  fieldType: string = '';


  constructor(private http: HttpClient, private router: Router, private fb: FormBuilder) {
    this.recoverForm = this.fb.group({
      input: ['', [Validators.required, this.validator]]
    });
  }

  ngOnInit(): void {
    const fromRecoverViaEmail = localStorage.getItem('fromRecoverViaEmail') || '';
    const fromRecoverViaSMS = localStorage.getItem('fromRecoverViaSMS') || '';

    if (fromRecoverViaEmail === 'true') {
      this.fieldLabel = 'Email address:';
      this.fieldType = 'email';
    } else if (fromRecoverViaSMS === 'true') {
      this.fieldLabel = 'Phone number:';
      this.fieldType = 'tel';
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
      mainpage: '/',
      login: '/login',
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
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  validator(control: AbstractControl): ValidationErrors | null {
    const fromRecoverViaEmail = localStorage.getItem('fromRecoverViaEmail') || '';
    const fromRecoverViaSMS = localStorage.getItem('fromRecoverViaSMS') || '';

    if (fromRecoverViaEmail === 'true') {
      const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
      return emailRegex.test(control.value) ? null : { invalidInput: true };
    }

    if (fromRecoverViaSMS === 'true') {
      const phoneRegex = /^[0-9]{10}$/;
      return phoneRegex.test(control.value) ? null : { invalidInput: true };
    }

    return null;
  }

  onRecover(): void {
    this.errorMessage = '';

    const fromRecoverViaEmail = localStorage.getItem('fromRecoverViaEmail') || '';
    const fromRecoverViaSMS = localStorage.getItem('fromRecoverViaSMS') || '';

    if (fromRecoverViaEmail === 'true') {
      const urlEncodedData = new URLSearchParams();

      urlEncodedData.append('email', this.recoverForm.get('input')?.value);

      const headers = new HttpHeaders({
        'Content-Type': 'application/x-www-form-urlencoded'
      });

      const apiUrl = window['env'].API_URL + '/api/recoverPasswordViaEmail';
      this.http.post<any>(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data) => {
            if (data.success) {
              localStorage.setItem('fromRecoverViaEmail', 'false');
              localStorage.setItem('fromRecoverPassword', 'true');
              this.router.navigate(['/login']);
            } else {
              this.errorMessage = 'No account was found with this email address.';
            }
          },
          (error) => {
            console.error('Error:', error);
          }
        );
    }
    else if (fromRecoverViaSMS === 'true') {
      const urlEncodedData = new URLSearchParams();

      urlEncodedData.append('phone', this.recoverForm.get('input')?.value);

      const headers = new HttpHeaders({
        'Content-Type': 'application/x-www-form-urlencoded'
      });

      const apiUrl = window['env'].API_URL + '/api/recoverPasswordViaSMS';
      this.http.post<{ success: boolean, message?: string }>(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          data => {
            if (data.success) {
              localStorage.setItem('fromRecoverViaSMS', 'false');
              localStorage.setItem('fromRecoverPassword', 'true');
              localStorage.setItem('phone', this.recoverForm.get('input')?.value);
              this.router.navigate(['/validate-phone']);
            } else {
              this.errorMessage = 'No account was found with this phone number.';
            }
          },
          error => {
            console.error('Error:', error);
            this.errorMessage = 'An unexpected error occurred. Please try again later.';
          }
        );
    }
  }
}