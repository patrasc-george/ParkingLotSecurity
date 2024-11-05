import { Component, OnInit } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router, ActivatedRoute } from '@angular/router';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {
  email: string = '';
  password: string = '';
  successMessage: string = '';
  errorMessage: string = '';

  constructor(
    private http: HttpClient,
    private router: Router,
    private route: ActivatedRoute
  ) { }

  ngOnInit(): void {
    this.route.queryParams.subscribe(params => {
      if (params['fromCreateAccount']) {
        this.successMessage = 'A validation email has been sent to your address.';
      } else if (params['fromValidate']) {
        this.successMessage = 'The account has been successfully validated.';
      } else if (params['fromRecoverPassword']) {
        this.successMessage = 'A password reset email has been sent to your address.';
      } else if (params['fromResetPassword'])
        this.successMessage = 'The password has been successfully changed.';
    });
  }

  onLogin() {
    this.successMessage = '';
    this.errorMessage = '';

    if (!this.email || !this.password) {
      this.errorMessage = 'Please enter both email and password.';
      return;
    }

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', this.email);
    urlEncodedData.append('password', this.password);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    this.http.post('http://localhost:8080/api/login', urlEncodedData.toString(), { headers })
      .subscribe(
        data => {
          console.log('Login response:', data);
          this.handleServerResponse(data);
        },
        error => {
          console.error('Error:', error);
          this.errorMessage = 'An error occurred. Please try again later.';
        }
      );
  }

  handleServerResponse(data: any) {
    this.successMessage = '';
    this.errorMessage = '';

    if (data.success === false) {
      this.errorMessage = data.message || 'Login failed. Please check your credentials.';
    } else if (data.success === true) {
      localStorage.setItem('subscriptionsData', JSON.stringify(data.subscriptions));
      localStorage.setItem('email', this.email);
      this.router.navigate(['/subscriptions']);
    } else {
      this.errorMessage = 'An unexpected response was received. Please try again.';
    }
  }
}
