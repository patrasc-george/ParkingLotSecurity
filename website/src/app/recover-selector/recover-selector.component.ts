import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Component, HostListener } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { Router } from '@angular/router';

@Component({
  selector: 'app-recover-selector',
  templateUrl: './recover-selector.component.html',
  styleUrl: './recover-selector.component.css'
})
export class RecoverSelectorComponent {
  dropdownVisible = false;
  validationForm: FormGroup;
  validationMethod: 'email' | 'sms' | null = null;
  apiURL: string = "{{API_URL}}";
  key: string = "{{POSTGRES_PASSWORD}}";
  
  constructor(private router: Router, private http: HttpClient, private fb: FormBuilder) {
    this.validationForm = this.fb.group({
      validationMethod: [null, Validators.required],
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

  onValidate(method: 'email' | 'sms'): void {
    if (method === 'email') {
      this.validateByEmail();
    } else if (method === 'sms') {
      this.validateBySMS();
    }
  }

  validateByEmail() {
    localStorage.setItem('fromRecoverViaEmail', 'true');
    this.router.navigate(['/recover-password']);
  }

  validateBySMS() {
    localStorage.setItem('fromRecoverViaSMS', 'true');
    this.router.navigate(['/recover-password']);
  }

}
