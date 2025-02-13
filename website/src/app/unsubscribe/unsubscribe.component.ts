import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, HostListener } from '@angular/core';
import { AbstractControl, FormBuilder, FormGroup, ValidationErrors, Validators } from '@angular/forms';
import { Router } from '@angular/router';

@Component({
  selector: 'app-unsubscribe',
  templateUrl: './unsubscribe.component.html',
  styleUrl: './unsubscribe.component.css'
})
export class UnsubscribeComponent {
  unsubscribeForm: FormGroup;
  successMessage: string = '';
  dropdownVisible = false;
  apiURL: string = "{{API_URL}}";
  key: string = "{{POSTGRES_PASSWORD}}";
  
  constructor(
    private fb: FormBuilder,
    private http: HttpClient,
    private router: Router
  ) {
    this.unsubscribeForm = this.fb.group({
      email: ['', [Validators.required, this.emailValidator]]
    });
  }

  ngOnInit(): void {
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

  unsubscribe() {
    this.successMessage = '';

    const email = this.unsubscribeForm.get('email')?.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    const apiUrl = this.apiURL  + '/api/unsubscribeNewsletter';
    this.http.post<any>(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data) => {
          if (data.success) {
            this.successMessage = 'You have successfully unsubscribed from the newsletter.'
          } else if (!data.success) {
            this.unsubscribeForm.get('email')?.setErrors({ emailExists: true });
            const emailField = document.querySelector(`[formControlName="email"]`);
            (emailField as HTMLElement)?.focus();
          }
        },
        error => {
          console.error('Error:', error);
        }
      );
  }

  emailValidator(control: AbstractControl): ValidationErrors | null {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
    return emailRegex.test(control.value) ? null : { invalidEmail: true };
  }
}
