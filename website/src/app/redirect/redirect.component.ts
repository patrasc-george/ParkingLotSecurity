import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, OnInit } from '@angular/core';
import { Router, ActivatedRoute } from '@angular/router';

@Component({
  selector: 'app-redirect',
  templateUrl: './redirect.component.html',
  styleUrls: ['./redirect.component.css']
})
export class RedirectComponent implements OnInit {
  remainingTime: number = 10;
  strokeOffset: number = 283;
  message: string = '';
  isRedirecting: boolean = false;

  constructor(private router: Router, private http: HttpClient, private route: ActivatedRoute) { }

  ngOnInit(): void {
    this.message += 'The operation has been successfully validated. You will be redirected to your account after the time expires.';
    this.startTimer();
  }

  startTimer(): void {
    const interval = setInterval(() => {
      if (this.remainingTime > 0) {
        this.remainingTime--;
        this.updateCircle();
      } else {
        clearInterval(interval);
        if (!this.isRedirecting) {
          this.redirectToAccount();
        }
      }
    }, 1000);
  }

  updateCircle(): void {
    const total = 283;
    this.strokeOffset = (this.remainingTime / 10) * total;
  }

  redirectToAccount(): void {
    if (this.isRedirecting) {
      return;
    }

    this.isRedirecting = true;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);

    const email = localStorage.getItem('email');
    if (email) {
      urlEncodedData.append('input', email);
    }
    urlEncodedData.append('fromRedirect', 'true');

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/login';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        data => {
          console.log('Login response:', data);
          this.handleServerResponse(data);
        },
        error => {
          console.error('Error:', error);
          this.router.navigate(['/login']);
        }
      );
  }

  handleServerResponse(data: any) {
    if (data.success === true) {
      localStorage.setItem('subscriptionsTable', JSON.stringify(data.subscriptionsTable));
      localStorage.setItem('name', data.name);
      localStorage.setItem('lastName', data.lastName);
      localStorage.setItem('email', data.email);
      localStorage.setItem('phone', data.phone);
      this.router.navigate(['/subscriptions']);
    } else {
      this.router.navigate(['/login']);
    }
  }
}
