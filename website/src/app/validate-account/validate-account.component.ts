import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { HttpClient, HttpHeaders } from '@angular/common/http';

@Component({
  selector: 'app-validate-account',
  templateUrl: './validate-account.component.html',
  styleUrls: ['./validate-account.component.css']
})
export class ValidateAccountComponent implements OnInit {
  apiURL: string = "{{API_URL}}";
  key: string = "{{POSTGRES_PASSWORD}}";
  
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
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', this.key);
    urlEncodedData.append('token', token);
    
    const headers = new HttpHeaders({'Content-Type': 'application/x-www-form-urlencoded' });

    const apiUrl = this.apiURL + '/api/validate';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => {
          if (data.success) {
            if (data.email != '') {
              localStorage.setItem('email', data.email);
            }
            localStorage.setItem('fromValidate', 'true');
            this.router.navigate(['/redirect']);
          } else {
            localStorage.setItem('invalidLink', 'true');
            this.router.navigate(['/login']);
          }
        },
        (error) => {
          console.error('Error:', error);
        }
      );
  }
}