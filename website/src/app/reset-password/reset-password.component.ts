import { Component, HostListener, OnInit } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { ActivatedRoute, Router } from '@angular/router';
import { FormBuilder, FormGroup, Validators, AbstractControl, ValidationErrors } from '@angular/forms';

@Component({
  selector: 'app-reset-password',
  templateUrl: './reset-password.component.html',
  styleUrls: ['./reset-password.component.css']
})
export class ResetPasswordComponent implements OnInit {
  resetPasswordForm: FormGroup;
  successMessage: string = '';
  errorMessage: string = '';
  dropdownVisible = false;
  apiURL: string = "{{API_URL}}";
  key: string = "{{POSTGRES_PASSWORD}}";
  
  constructor(
    private fb: FormBuilder,
    private http: HttpClient,
    private route: ActivatedRoute,
    private router: Router
  ) {
    this.resetPasswordForm = this.fb.group({
      password: ['', [Validators.required, this.passwordValidator]],
      confirmPassword: ['', [Validators.required, this.matchPasswordValidator.bind(this)]]
    });
  }

  ngOnInit(): void {
    this.route.queryParams.subscribe(params => {
      this.resetPasswordForm.addControl('token', this.fb.control(params['token'] || ''));
    });

    if (localStorage.getItem('fromValidatePhone') === 'true') {
      localStorage.setItem('fromValidatePhone', 'false');
    } else {
      this.verifyToken()
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
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = this.apiURL  + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  verifyToken() {
    this.successMessage = '';
    this.errorMessage = '';

    const token = this.resetPasswordForm.get('token')?.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('token', token);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    const apiUrl = this.apiURL  + '/api/verifyResetPasswordToken';
    this.http.post<any>(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data) => {
          if (data.success) {
            if (data.email != '') {
              localStorage.setItem('email', data.email);
            }
          }
          else {
            localStorage.setItem('invalidLink', 'true');
            this.router.navigate(['/login']);
          }
        },
        error => {
          console.error('Error:', error);
          this.errorMessage = 'An error occurred. Please try again later.';
        }
      );
  }

  onResetPassword() {
    this.successMessage = '';
    this.errorMessage = '';

    const newPassword = this.resetPasswordForm.get('password')?.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('email', localStorage.getItem('email') || '');
    urlEncodedData.append('newPassword', newPassword);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    const apiUrl = this.apiURL  + '/api/resetPassword';
    this.http.post<any>(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data) => {
          localStorage.setItem('fromResetPassword', 'true');
          this.router.navigate(['/redirect']);
        },
        error => {
          console.error('Error:', error);
          this.errorMessage = 'An error occurred. Please try again later.';
        }
      );
  }

  passwordValidator(control: AbstractControl): ValidationErrors | null {
    const password = control.value || '';
    const errors: ValidationErrors = {};

    if (!/[A-Z]/.test(password)) errors['uppercaseRequired'] = true;
    if (!/\d/.test(password)) errors['numberRequired'] = true;
    if (password.length < 8) errors['minLength'] = true;

    return Object.keys(errors).length ? errors : null;
  }

  matchPasswordValidator(control: AbstractControl): ValidationErrors | null {
    const password = this.resetPasswordForm?.get('password')?.value;
    return control.value === password ? null : { passwordMismatch: true };
  }
}
