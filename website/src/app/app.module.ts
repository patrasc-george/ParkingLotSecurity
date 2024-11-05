import { NgModule } from '@angular/core';
import { BrowserModule, provideClientHydration } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { MainpageComponent } from './mainpage/mainpage.component';
import { CreateAccountComponent } from './create-subscription/create-subscription.component';
import { ValidateAccountComponent } from './validate-account/validate-account.component';
import { LoginComponent } from './login/login.component';
import { RecoverPasswordComponent } from './recover-password/recover-password.component';
import { ResetPasswordComponent } from './reset-password/reset-password.component';
import { SubscriptionsComponent } from './subscriptions/subscriptions.component';
import { SubscriptionComponent } from './subscription/subscription.component';
import { AccountComponent } from './account/account.component';

@NgModule({
  declarations: [
    AppComponent,
    MainpageComponent,
    CreateAccountComponent,
    ValidateAccountComponent,
    LoginComponent,
    RecoverPasswordComponent,
    ResetPasswordComponent,
    SubscriptionsComponent,
    SubscriptionComponent,
    AccountComponent
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    FormsModule,
    ReactiveFormsModule,
    AppRoutingModule
  ],
  providers: [
    provideClientHydration()
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
