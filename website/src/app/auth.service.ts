import { Injectable } from '@angular/core';

@Injectable({
    providedIn: 'root'
})
export class AuthService {

    constructor() { }

    isAuthenticated(): boolean {
        return !!localStorage.getItem('email');
    }

    logout(): void {
        localStorage.removeItem('name');
        localStorage.removeItem('lastName');
        localStorage.removeItem('email');
        localStorage.removeItem('password');
        localStorage.removeItem('phone');
        localStorage.removeItem('admin');
    }
}
