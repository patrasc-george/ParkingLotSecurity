import { Component, OnInit } from '@angular/core';
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
      this.resetPasswordForm.addControl('email', this.fb.control(params['email'] || ''));
    });
  }

  onResetPassword() {
    this.successMessage = '';
    this.errorMessage = '';

    const email = this.resetPasswordForm.get('email')?.value;
    const newPassword = this.resetPasswordForm.get('password')?.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', email);
    urlEncodedData.append('newPassword', newPassword);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    this.http.post('http://localhost:8080/api/resetPassword', urlEncodedData.toString(), { headers })
      .subscribe(
        () => {
          this.router.navigate(['/login'], { queryParams: { fromResetPassword: true } });
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
