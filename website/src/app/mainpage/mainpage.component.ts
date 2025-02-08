import { Component, HostListener } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';

@Component({
  selector: 'app-mainpage',
  templateUrl: './mainpage.component.html',
  styleUrls: ['./mainpage.component.css']
})
export class MainpageComponent {
  licensePlate: string = '';
  fileName: string = '';
  errorMessage: string = '';
  successMessage: string = '';
  selectedFile: File | null = null;
  dropdownVisible = false;

  constructor(private http: HttpClient, private router: Router) { }

  onLogin() {
    this.router.navigate(['/login']);
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

    if (route === "/") {
      window.location.reload();
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
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
    urlEncodedData.append('email', email);

    const headers = new HttpHeaders({
      'Content-Type': 'application/x-www-form-urlencoded'
    });

    const apiUrl = window['env'].API_URL + '/api/subscribeNewsletter';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe();
  }

  onUploadQrCode() {
    const qrInput = document.getElementById('qrInput') as HTMLInputElement;
    if (qrInput) {
      qrInput.click();
    }
  }

  onFileSelected(event: Event) {
    const input = event.target as HTMLInputElement;
    if (input.files && input.files.length > 0) {
      this.selectedFile = input.files[0];
      this.licensePlate = '';
      this.fileName = this.selectedFile.name;
      this.errorMessage = '';
    }
  }

  onPay() {
    this.errorMessage = '';
    this.successMessage = '';

    const apiUrl = window['env'].API_URL + '/api/endpoint';

    if (!this.licensePlate && !this.selectedFile) {
      this.errorMessage = 'Please enter a license plate number or upload a QR code.';
      console.error('Error: No license plate or QR code uploaded.');
      return;
    }

    if (this.licensePlate) {
      this.selectedFile = null;
      this.fileName = '';

      const urlEncodedData = new URLSearchParams();
      urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
      urlEncodedData.append('licensePlate', this.licensePlate);

      const headers = new HttpHeaders({
        'Content-Type': 'application/x-www-form-urlencoded'
      });

      console.log('Sending license plate:', this.licensePlate);

      this.http.post(apiUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          data => {
            console.log('Response from server (license plate):', data);
            this.handleServerResponse(data);
          },
          error => {
            console.error('Error during POST request for license plate:', error);
            this.errorMessage = error.message || 'An error occurred while processing the license plate.';
            const errorData = error.error ? error.error : { success: false };
            this.handleServerResponse(errorData);
          }
        );
    } else if (this.selectedFile) {
      const formData = new FormData();
      formData.append('qrCodeImage', this.selectedFile, this.selectedFile.name);

      console.log('Sending QR code image:', this.selectedFile.name);

      this.http.post(apiUrl, formData)
        .subscribe(
          data => {
            console.log('Response from server (QR code):', data);
            this.handleServerResponse(data);
          },
          error => {
            console.error('Error during POST request for QR code:', error);
            this.errorMessage = error.message || 'An error occurred while processing the QR code.';
            const errorData = error.error ? error.error : { success: false };
            this.handleServerResponse(errorData);
          }
        );
    }
  }

  handleServerResponse(data: any) {
    this.errorMessage = '';
    this.successMessage = '';

    if (data.success === false) {
      this.errorMessage = data.message;
    } else if (data.success === true) {
      this.successMessage = data.message
    } else {
      this.errorMessage = 'An unexpected response was received. Please try again.';
    }
  }
}
