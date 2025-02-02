import { Component, ElementRef, HostListener, OnInit, ViewChild } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';
import { FormBuilder, FormGroup, Validators, AbstractControl, ValidationErrors } from '@angular/forms';

@Component({
  selector: 'app-account',
  templateUrl: './account.component.html',
  styleUrls: ['./account.component.css']
})
export class AccountComponent implements OnInit {
  name: string = '';
  formName: string = '';
  formLastName: string = '';
  phone: string = '';
  email: string = '';
  password: string = '';
  isReadOnly: boolean = true;
  isAdmin: boolean = false;

  dropdownVisible = false;

  formVisibleAccount: boolean = false;
  formVisibleSecurity: boolean = false;

  signUpForm!: FormGroup;
  @ViewChild('formNameInput') formNameInput!: ElementRef;
  @ViewChild('formLastNameInput') formLastNameInput!: ElementRef;
  @ViewChild('emailInput') emailInput!: ElementRef;
  @ViewChild('passwordInput') passwordInput!: ElementRef;
  @ViewChild('phoneInput') phoneInput!: ElementRef;

  editVisibility: { [key: string]: boolean } = {
    formName: true,
    formLastName: true,
    email: true,
    password: true,
    phone: true,
  };

  constructor(private fb: FormBuilder, private router: Router, private http: HttpClient) { }

  ngOnInit(): void {
    this.isAdmin = localStorage.getItem('admin') === 'true';

    this.signUpForm = this.fb.group({
      formName: [{ value: '', disabled: true }, [Validators.required, this.nameValidator]],
      formLastName: [{ value: '', disabled: true }, [Validators.required, this.nameValidator]],
      phone: [{ value: '', disabled: true }, [Validators.required, this.phoneValidator]],
      email: [{ value: '', disabled: true }, [Validators.required, this.emailValidator]],
      password: [{ value: '', disabled: true }, [Validators.required, this.passwordValidator]]
    });

    this.name = localStorage.getItem('name') || 'Guest';
    this.formName = localStorage.getItem('name') || '';
    this.formLastName = localStorage.getItem('lastName') || '';
    this.phone = localStorage.getItem('phone') || '';
    this.email = localStorage.getItem('email') || '';
    this.password = "passwordpassword";

    this.signUpForm.patchValue({
      formName: this.formName,
      formLastName: this.formLastName,
      phone: this.phone,
      email: this.email,
      password: this.password,
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
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  redirectToSubscriptions(event: Event): void {
    event.preventDefault();
    this.router.navigate(['/subscriptions']);
  }

  resetFields() {
    this.signUpForm.patchValue({
      formName: this.formName,
      formLastName: this.formLastName,
      phone: this.phone,
      email: this.email,
      password: this.password
    });

    Object.keys(this.signUpForm.controls).forEach(field => {
      this.signUpForm.get(field)?.disable();
    });

    this.editVisibility = {
      formName: true,
      formLastName: true,
      email: true,
      password: true,
      phone: true,
    };
  }

  toggleForm(section: string): void {
    if (section === 'account') {
      this.resetFields();
      this.formVisibleAccount = !this.formVisibleAccount;
      if (this.formVisibleAccount) {
        this.formVisibleSecurity = false;
      }
    } else if (section === 'security') {
      this.resetFields();
      this.formVisibleSecurity = !this.formVisibleSecurity;
      if (this.formVisibleSecurity) {
        this.formVisibleAccount = false;
      }
    }
  }

  activateField(field: string): void {
    const control = this.signUpForm.get(field);

    if (control) {
      control.enable();
      this.signUpForm.patchValue({ [field]: '' });
      this.editVisibility[field] = false;

      if (field === 'formName') {
        setTimeout(() => this.formNameInput.nativeElement.focus(), 0);
      } else if (field === 'formLastName') {
        setTimeout(() => this.formLastNameInput.nativeElement.focus(), 0);
      } else if (field === 'email') {
        setTimeout(() => this.emailInput.nativeElement.focus(), 0);
      } else if (field === 'password') {
        setTimeout(() => this.passwordInput.nativeElement.focus(), 0);
      } else if (field === 'phone') {
        setTimeout(() => this.phoneInput.nativeElement.focus(), 0);
      }
    }
  }

  saveAccoountInformations() {
    if (this.signUpForm.invalid) {
      return;
    }

    const { formName, formLastName, email, password, phone } = this.signUpForm.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', this.email);
    if (formName) urlEncodedData.append('newName', formName);
    if (formLastName) urlEncodedData.append('newLastName', formLastName);
    if (phone) urlEncodedData.append('newPhone', phone);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/updateAccountInformation';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => {
          if (data.success === false) {
            this.signUpForm.get('phone')?.setErrors({ phoneExists: true });
            const phoneField = document.querySelector(`[formControlName="phone"]`);
            (phoneField as HTMLElement)?.focus();
          } else if (data.success === true) {
            if (formName) localStorage.setItem('name', formName);
            if (formLastName) localStorage.setItem('lastName', formLastName);
            if (phone) localStorage.setItem('phone', phone);
            window.location.reload();
          }
        },
        error => {
          console.error('Error:', error);
        }
      );
  }

  saveSecurity() {
    if (this.signUpForm.invalid) {
      return;
    }

    const { formName, formLastName, email, password, phone } = this.signUpForm.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', this.email);
    urlEncodedData.append('admin', this.isAdmin.toString());
    if (email) urlEncodedData.append('newEmail', email);
    if (password) urlEncodedData.append('newPassword', password);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/updateAccount';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => {
          if (data.success === false) {
            this.signUpForm.get('email')?.setErrors({ emailExists: true });
            const emailField = document.querySelector(`[formControlName="email"]`);
            (emailField as HTMLElement)?.focus();
          } else if (data.success === true) {
            if (this.isAdmin) {
              if (email) localStorage.setItem('email', email);
              if (password) localStorage.setItem('password', password);
              window.location.reload();
            } else {
              localStorage.setItem('fromAccount', 'true');
              this.router.navigate(['/validation-selector']);
            }
          }
        },
        error => {
          console.error('Error:', error);
        }
      );
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

  phoneValidator(control: AbstractControl): ValidationErrors | null {
    const phoneRegex = /^[0-9]{10}$/;
    return phoneRegex.test(control.value) ? null : { invalidPhone: true };
  }
}
