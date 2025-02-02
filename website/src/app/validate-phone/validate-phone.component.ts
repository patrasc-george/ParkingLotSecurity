import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, HostListener } from '@angular/core';
import { Router } from '@angular/router';
import { AuthService } from '../auth.service';

@Component({
  selector: 'app-validate-phone',
  templateUrl: './validate-phone.component.html',
  styleUrl: './validate-phone.component.css'
})
export class ValidatePhoneComponent {
  code = ['', '', '', '', '', ''];
  dropdownVisible = false;
  errorMessage: string = '';
  timer: number = 10;
  timerInterval: any;
  email: string = '';
  phone: string = '';

  constructor(
    private http: HttpClient,
    private router: Router,
    private authService: AuthService
  ) {
    this.email = localStorage.getItem('email') || '';
    this.phone = localStorage.getItem('phone') || '';
    this.startTimer();
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
      login: '/validate-phone',
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

  onKeyDown(event: KeyboardEvent, index: number): void {
    const key = event.key;

    if (!/^[0-9]$/.test(key) && key !== 'Backspace') {
      event.preventDefault();
      return;
    }

    if (/^[0-9]$/.test(key)) {
      this.code[index] = key;

      if (index < 5) {
        const nextInput = document.getElementsByClassName('code-input')[index + 1] as HTMLInputElement;
        nextInput.focus();
      }

      event.preventDefault();
    }

    if (key === 'Backspace') {
      this.code[index] = '';

      if (index > 0) {
        const prevInput = document.getElementsByClassName('code-input')[index - 1] as HTMLInputElement;
        prevInput.focus();
      }

      event.preventDefault();
    }
  }

  validateToken(): void {
    const token = this.code.join('');
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('token', token);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    if (localStorage.getItem('fromValidationSelector') === 'true') {
      const apiUrl = window['env'].API_URL + '/api/validate';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data) => {
            this.handleServerResponse(data);
          },
          (error) => {
            console.error('Error:', error);
            this.router.navigate(['/redirect']);
          }
        );
    } else if (localStorage.getItem('fromRecoverPassword') === 'true') {
      const apiUrl = window['env'].API_URL + '/api/verifyResetPasswordToken';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data) => {
            this.handleServerResponse(data);
          },
          (error) => {
            console.error('Error:', error);
            this.router.navigate(['/reset-password']);
          }
        );
    } else if (localStorage.getItem('fromAccount') === 'true') {
      const apiUrl = window['env'].API_URL + '/api/validateUpdate';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (data) => {
            this.handleServerResponse(data);
          },
          (error) => {
            console.error('Error:', error);
            this.router.navigate(['/redirect']);
          }
        );
    }
  }

  handleServerResponse(data: any) {
    if (data.success === false) {
      this.errorMessage = 'The code entered is not correct.';
      console.error(this.errorMessage);
      return;
    } else if (data.success === true) {
      if (data.email != '') {
        localStorage.setItem('email', data.email);
      }
      if (localStorage.getItem('fromValidationSelector') === 'true') {
        localStorage.setItem('fromValidationSelector', 'false');
        localStorage.setItem('fromValidate', 'true');
        this.router.navigate(['/redirect']);
      } else if (localStorage.getItem('fromRecoverPassword') === 'true') {
        localStorage.setItem('fromRecoverPassword', 'false');
        localStorage.setItem('fromValidatePhone', 'true');
        this.router.navigate(['/reset-password']);
      } else if (localStorage.getItem('fromAccount') === 'true') {
        this.router.navigate(['/redirect']);
      }
    }
  }

  startTimer(): void {
    this.timer = 10;
    this.timerInterval = setInterval(() => {
      if (this.timer > 0) {
        this.timer--;
      } else {
        clearInterval(this.timerInterval);
      }
    }, 1000);
  }

  resendSMS(): void {
    this.startTimer();

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', this.email);
    urlEncodedData.append('phone', this.phone);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    if (localStorage.getItem('fromValidationSelector') === 'true') {
      const apiUrl = window['env'].API_URL + '/api/resendValidateSMS';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (error) => {
            console.error('Error:', error);
          }
        );
    } else if (localStorage.getItem('fromRecoverPassword') === 'true') {
      const apiUrl = window['env'].API_URL + '/api/resendRecoverPassword';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (error) => {
            console.error('Error:', error);
          }
        );
    } else if (localStorage.getItem('fromAccount') === 'true') {
      const apiUrl = window['env'].API_URL + '/api/resendValidateUpdateSMS';
      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          (error) => {
            console.error('Error:', error);
          }
        );
    }
  }
}
