import { Component, HostListener, OnInit } from '@angular/core';
import { AbstractControl, FormBuilder, FormGroup, ValidationErrors, Validators } from '@angular/forms';
import { AuthService } from '../auth.service';
import { Router } from '@angular/router';
import { HttpClient, HttpHeaders } from '@angular/common/http';

@Component({
  selector: 'app-contact',
  templateUrl: './contact.component.html',
  styleUrls: ['./contact.component.css']
})
export class ContactComponent implements OnInit {
  successMessage: string = '';
  dropdownVisible: boolean = false;
  name: string = "";
  isAuthenticated: boolean = false;
  isAdmin: boolean = false;

  contactForm!: FormGroup;

  constructor(private router: Router, private authService: AuthService, private fb: FormBuilder, private http: HttpClient) { }

  ngOnInit(): void {
    this.isAuthenticated = this.authService.isAuthenticated();
    this.name = localStorage.getItem('name') || '';
    this.isAdmin = localStorage.getItem('admin') === 'true';

    this.contactForm = this.fb.group({
      email: [localStorage.getItem('email') || '', [Validators.required, this.emailValidator]],
      subject: ['', Validators.required],
      message: ['', Validators.required]
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
      mainpage: '/',
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

  redirectToSubscriptions(event: Event) {
    event.preventDefault();
    if (this.isAuthenticated) {
      this.router.navigate(['/subscriptions']);
    } else {
      this.router.navigate(['/']);
    }
  }

  onSubmit(): void {
    this.successMessage = '';

    const { email, subject, message } = this.contactForm.value;

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('email', email);
    urlEncodedData.append('subject', subject);
    urlEncodedData.append('message', message);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/contact';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => this.handleServerResponse(data),
        error => {
          console.error('Error:', error);
        }
      );
  }

  handleServerResponse(data: any) {
    if (data.success === true) {
      this.successMessage = 'Your message has been sent successfully.';
      this.contactForm.reset();
    }
  }

  emailValidator(control: AbstractControl): ValidationErrors | null {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]{2,}$/;
    return emailRegex.test(control.value) ? null : { invalidEmail: true };
  }
}
