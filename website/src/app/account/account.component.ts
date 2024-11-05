import { Component } from '@angular/core';
import { HttpClient } from '@angular/common/http';
@Component({
  selector: 'app-account',
  templateUrl: './account.component.html',
  styleUrl: './account.component.css'
})
export class AccountComponent {
  currentPasswordName: string = '';
  newName: string = '';
  currentPassword: string = '';
  newPassword: string = '';
  errorMessage: string = '';
  successMessage: string = '';

  constructor(private http: HttpClient) {}

  updateEmail() {
    this.errorMessage = '';
    this.successMessage = '';

    if (!this.validateEmail(this.newName)) {
      this.errorMessage = "Please enter a valid email address.";
      return;
    }

    if (!this.currentPasswordName || !this.newName) {
      this.errorMessage = "Please enter both current password and new email.";
      return;
    }

    const name = localStorage.getItem('name');
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('name', name!);
    urlEncodedData.append('currentPassword', this.currentPasswordName);
    urlEncodedData.append('newName', this.newName);

    this.http.post('http://localhost:8080/api/updateName', urlEncodedData.toString(), {
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
    }).subscribe(
      (response: any) => {
        if (response.success) {
          localStorage.setItem('name', this.newName);
          this.successMessage = 'Email address updated successfully.';
        } else {
          this.errorMessage = response.message;
        }
      },
      (error) => {
        console.error('Error:', error);
        this.errorMessage = 'An error occurred. Please try again later.';
      }
    );
  }

  updatePassword() {
    this.errorMessage = '';
    this.successMessage = '';

    if (!this.currentPassword || !this.newPassword) {
      this.errorMessage = "Please enter both current password and new password.";
      return;
    }

    const name = localStorage.getItem('name');
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('name', name!);
    urlEncodedData.append('currentPassword', this.currentPassword);
    urlEncodedData.append('newPassword', this.newPassword);

    this.http.post('http://localhost:8080/api/updatePassword', urlEncodedData.toString(), {
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
    }).subscribe(
      (response: any) => {
        if (response.success) {
          this.successMessage = 'Password updated successfully.';
        } else {
          this.errorMessage = 'The current password is incorrect.';
        }
      },
      (error) => {
        console.error('Error:', error);
        this.errorMessage = 'An error occurred. Please try again later.';
      }
    );
  }

  validateEmail(email: string): boolean {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
  }
}
