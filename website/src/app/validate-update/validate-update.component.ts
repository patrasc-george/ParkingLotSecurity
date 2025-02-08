import { Component, OnInit } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { HttpClient, HttpHeaders } from '@angular/common/http';

@Component({
  selector: 'app-validate-update',
  templateUrl: './validate-update.component.html',
  styleUrls: ['./validate-update.component.css']
})
export class ValidateUpdateComponent implements OnInit {

  constructor(
    private route: ActivatedRoute,
    private http: HttpClient,
    private router: Router
  ) { }

  ngOnInit(): void {
    this.route.queryParams.subscribe(params => {
      const token = params['token'];
      if (token) {
        this.validateUpdate(token);
      }
    });
  }

  validateUpdate(token: string): void {
    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('key', window['env'].POSTGRES_PASSWORD);
    urlEncodedData.append('token', token);

    const headers = new HttpHeaders({ 'Content-Type': 'application/x-www-form-urlencoded' });

    const apiUrl = window['env'].API_URL + '/api/validateUpdate';
    this.http.post(apiUrl, urlEncodedData.toString(), { headers })
      .subscribe(
        (data: any) => {
          if (data.success) {
            if (data.email != '') {
              localStorage.setItem('email', data.email);
            }
            localStorage.setItem('fromValidate', 'true');
            this.router.navigate(['/redirect']);
          }
        },
        error => {
          console.error('Error:', error);
        }
      );
  }
}
