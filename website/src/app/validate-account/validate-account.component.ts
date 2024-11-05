import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-validate-account',
  templateUrl: './validate-account.component.html',
  styleUrls: ['./validate-account.component.css']
})
export class ValidateAccountComponent implements OnInit {
  constructor(
    private route: ActivatedRoute,
    private http: HttpClient,
    private router: Router
  ) { }

  ngOnInit(): void {
    this.route.queryParams.subscribe(params => {
      const token = params['token'];
      if (token) {
        this.validateAccount(token);
      }
    });
  }

  validateAccount(token: string): void {
    const url = `http://localhost:8080/api/validate?token=${token}`;

    this.http.post<{ success: boolean, message?: string }>(url, {})
      .subscribe(
        (data) => {
          this.router.navigate(['/login'], { queryParams: { fromValidate: true } });
        },
        (error) => {
          console.error('Error:', error);
          this.router.navigate(['/login']);
        }
      );
  }
}
