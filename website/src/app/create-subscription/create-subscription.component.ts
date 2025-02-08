import { Component, HostListener } from '@angular/core';
import { FormBuilder, FormGroup, Validators, AbstractControl, ValidationErrors } from '@angular/forms';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';

@Component({
  selector: 'app-create-subscription',
  templateUrl: './create-subscription.component.html',
  styleUrls: ['./create-subscription.component.css']
})
export class CreateAccountComponent {
  signUpForm: FormGroup;
  dropdownVisible = false;
  captchaResponse: string | null = '';

  constructor(
    private fb: FormBuilder,
    private http: HttpClient,
    private router: Router
  ) {
    this.signUpForm = this.fb.group({
      name: ['', [Validators.required, this.nameValidator]],
      lastName: ['', [Validators.required, this.nameValidator]],
      email: ['', [Validators.required, this.emailValidator]],
      password: ['', [Validators.required, this.passwordValidator]],
      confirmPassword: ['', [Validators.required, this.matchPasswordValidator.bind(this)]],
      phone: ['', [Validators.required, this.phoneValidator]],
      terms: [false, [Validators.requiredTrue]],
      privacy: [false, [Validators.requiredTrue]],
      cookies: [false, [Validators.requiredTrue]],
      captcha: [null, [Validators.required]]
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

  policyNavigateTo(destination: string, event: MouseEvent): void {
    event.preventDefault();
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

  captchaResolved(response: string | null) {
    if (response) {
      this.captchaResponse = response;
    } else {
      this.captchaResponse = '';
    }
  }

  onSignUp() {
    if (!this.captchaResponse) {
      this.signUpForm.get('captcha')?.markAsTouched();
    }

    Object.keys(this.signUpForm.controls).forEach(field => {
      const control = this.signUpForm.get(field);
      control?.markAsTouched({ onlySelf: true });
    });

    const firstInvalidField = Object.keys(this.signUpForm.controls).find(field => {
      const control = this.signUpForm.get(field);
      return control && control.invalid;
    });

    if (firstInvalidField) {
      let elementToFocus: HTMLElement | null = null;

      if (firstInvalidField !== 'captcha') {
        elementToFocus = document.querySelector(`[formControlName="${firstInvalidField}"]`) as HTMLElement;
      }

      if (elementToFocus) {
        elementToFocus.scrollIntoView({ behavior: 'smooth', block: 'center' });
        elementToFocus.focus();
        return;
      }
    }

    if (this.signUpForm.get('captcha')?.invalid) {
      const captchaField = document.querySelector('.recaptcha-container') as HTMLElement;
      captchaField?.scrollIntoView({ behavior: 'smooth', block: 'center' });
      captchaField?.focus();
    }

    const { name, lastName, email, password, confirmPassword, phone } = this.signUpForm.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
    urlEncodedData.append('name', name);
    urlEncodedData.append('lastName', lastName);
    urlEncodedData.append('email', email);
    urlEncodedData.append('password', password);
    urlEncodedData.append('phone', phone);
    urlEncodedData.append('captchaToken', this.captchaResponse || '');

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/createAccount';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        data => {
          localStorage.setItem('name', name);
          localStorage.setItem('lastName', lastName);
          localStorage.setItem('email', email);
          localStorage.setItem('phone', phone);
          this.handleServerResponse(data)
        },
        error => {
          console.error('Error:', error);
        }
      );
  }

  handleServerResponse(data: any) {
    if (data.success === false) {
      this.captchaResponse = '';
      this.signUpForm.get('captcha')?.setErrors({ captchaInvalid: true });
      window.grecaptcha.reset();

      if (data.message === 'An account with this email address already exists.') {
        this.signUpForm.get('email')?.setErrors({ emailExists: true });
        const emailField = document.querySelector(`[formControlName="email"]`);
        emailField?.scrollIntoView({ behavior: 'smooth', block: 'center' });
        (emailField as HTMLElement)?.focus();
      } else if (data.message === 'An account with this phone number already exists.') {
        this.signUpForm.get('phone')?.setErrors({ phoneExists: true });
        const phoneField = document.querySelector(`[formControlName="phone"]`);
        phoneField?.scrollIntoView({ behavior: 'smooth', block: 'center' });
        (phoneField as HTMLElement)?.focus();
      }
    } else if (data.success === true) {
      localStorage.setItem('fromCreateSubscription', 'true');
      this.router.navigate(['/validation-selector']);
    }
  }

  nameValidator(control: AbstractControl): ValidationErrors | null {
    const name = control.value;
    return name && name.length >= 3 ? null : { invalidName: true };
  }

  emailValidator(control: AbstractControl): ValidationErrors | null {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
    return emailRegex.test(control.value) ? null : { invalidEmail: true };
  }

  passwordValidator(control: AbstractControl): ValidationErrors | null {
    const password = control.value || '';
    const errors: ValidationErrors = {};

    if (!/[A-Z]/.test(password)) errors['uppercaseRequired'] = true;
    if (!/\d/.test(password)) errors['numberRequired'] = true;
    if (password.length < 8) errors['minLength'] = true;

    return Object.keys(errors).length > 0 ? errors : null;
  }

  matchPasswordValidator(control: AbstractControl): ValidationErrors | null {
    const password = this.signUpForm?.get('password')?.value;
    const confirmPassword = control.value;

    if (password === '') {
      return { passwordMismatch: true };
    }

    if (confirmPassword && password !== confirmPassword) {
      return { passwordMismatch: true };
    }

    return null;
  }

  phoneValidator(control: AbstractControl): ValidationErrors | null {
    const phoneRegex = /^[0-9]{10}$/;
    return phoneRegex.test(control.value) ? null : { invalidPhone: true };
  }
}
