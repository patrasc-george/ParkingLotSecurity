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
        localStorage.removeItem('email');
        localStorage.removeItem('password');
    }
}
