import { Component } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';
import { FormGroup, FormBuilder, Validators, AbstractControl, ValidationErrors } from '@angular/forms';

@Component({
  selector: 'app-recover-password',
  templateUrl: './recover-password.component.html',
  styleUrls: ['./recover-password.component.css']
})
export class RecoverPasswordComponent {
  recoverForm: FormGroup;
  errorMessage: string | null = null;

  constructor(private http: HttpClient, private router: Router, private fb: FormBuilder) {
    this.recoverForm = this.fb.group({
      email: ['', [Validators.required, this.emailValidator]]
    });
  }

  emailValidator(control: AbstractControl): ValidationErrors | null {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
    return emailRegex.test(control.value) ? null : { invalidEmail: true };
  }

  onRecover(): void {
    this.errorMessage = '';

    const url = 'http://localhost:8080/api/recoverPassword';
    const urlEncodedData = new URLSearchParams();

    urlEncodedData.append('email', this.recoverForm.get('email')?.value);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    this.http.post<{ success: boolean, message?: string }>(url, urlEncodedData.toString(), { headers })
      .subscribe(
        data => {
          if (data.success) {
            this.router.navigate(['/login'], { queryParams: { fromRecoverPassword: true } });
          } else {
            this.errorMessage = 'No account was found with this email address.';
          }
        },
        error => {
          console.error('Error:', error);
          this.errorMessage = 'An unexpected error occurred. Please try again later.';
        }
      );
  }
}
