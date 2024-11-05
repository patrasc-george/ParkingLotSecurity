import { Component } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Router } from '@angular/router';

@Component({
  selector: 'app-mainpage',
  templateUrl: './mainpage.component.html',
  styleUrls: ['./mainpage.component.css']
})
export class MainpageComponent {
  serverUrl: string = 'http://localhost:8080/api/endpoint';
  licensePlate: string = '';
  fileName: string = '';
  errorMessage: string = '';
  successMessage: string = '';
  selectedFile: File | null = null;

  constructor(private http: HttpClient, private router: Router) { }

  onLogin() {
    this.router.navigate(['/login']);
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

    if (!this.licensePlate && !this.selectedFile) {
      this.errorMessage = 'Please enter a license plate number or upload a QR code.';
      return;
    }

    if (this.licensePlate) {
      this.selectedFile = null;
      this.fileName = '';

      const urlEncodedData = new URLSearchParams();
      urlEncodedData.append('licensePlate', this.licensePlate);

      const headers = new HttpHeaders({
        'Content-Type': 'application/x-www-form-urlencoded'
      });

      this.http.post(this.serverUrl, urlEncodedData.toString(), { headers })
        .subscribe(
          data => {
            this.handleServerResponse(data);
          },
          error => {
            console.error('Error:', error);
            const errorData = error.error ? error.error : { success: false };
            this.handleServerResponse(errorData);
          }
        );
    } else if (this.selectedFile) {
      const formData = new FormData();
      formData.append('qrCodeImage', this.selectedFile, this.selectedFile.name);

      this.http.post(this.serverUrl, formData)
        .subscribe(
          data => {
            this.handleServerResponse(data);
          },
          error => {
            console.error('Error:', error);
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
