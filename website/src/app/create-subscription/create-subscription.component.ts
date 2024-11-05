import { Component } from '@angular/core';
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
  errorMessage: string = '';

  constructor(
    private fb: FormBuilder,
    private http: HttpClient,
    private router: Router
  ) {
    this.signUpForm = this.fb.group({
      name: ['', [Validators.required, this.nameValidator]],
      email: ['', [Validators.required, this.emailValidator]],
      password: ['', [Validators.required, this.passwordValidator]],
      confirmPassword: ['', [Validators.required, this.matchPasswordValidator.bind(this)]],
      phone: ['', [Validators.required, this.phoneValidator]]
    });
  }

  onSignUp() {
    this.errorMessage = '';

    if (this.signUpForm.invalid) {
      return;
    }

    const { name, email, password, confirmPassword, phone } = this.signUpForm.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('name', name);
    urlEncodedData.append('email', email);
    urlEncodedData.append('password', password);
    urlEncodedData.append('phone', phone);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    this.http.post('http://localhost:8080/api/createAccount', urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => this.handleServerResponse(data),
        error => {
          console.error('Error:', error);
          this.errorMessage = 'An unexpected response was received. Please try again.';
        }
      );
  }

  handleServerResponse(data: any) {
    if (data.success === false) {
      this.errorMessage = data.message;
    } else if (data.success === true) {
      this.router.navigate(['/login'], { queryParams: { fromCreateAccount: true } });
    } else {
      this.errorMessage = 'An unexpected response was received. Please try again.';
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
    return control.value === password ? null : { passwordMismatch: true };
  }

  phoneValidator(control: AbstractControl): ValidationErrors | null {
    const phoneRegex = /^[0-9]{10}$/;
    return phoneRegex.test(control.value) ? null : { invalidPhone: true };
  }
}
