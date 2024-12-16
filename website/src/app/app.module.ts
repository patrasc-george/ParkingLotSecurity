import { NgModule } from '@angular/core';
import { BrowserModule, provideClientHydration } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { RecaptchaModule } from 'ng-recaptcha';
import { MainpageComponent } from './mainpage/mainpage.component';
import { CreateAccountComponent } from './create-subscription/create-subscription.component';
import { ValidationSelectorComponent } from './validation-selector/validation-selector.component';
import { ValidateAccountComponent } from './validate-account/validate-account.component';
import { ValidatePhoneComponent } from './validate-phone/validate-phone.component';
import { LoginComponent } from './login/login.component';
import { RecoverSelectorComponent } from './recover-selector/recover-selector.component';
import { RecoverPasswordComponent } from './recover-password/recover-password.component';
import { ResetPasswordComponent } from './reset-password/reset-password.component';
import { SubscriptionsComponent } from './subscriptions/subscriptions.component';
import { SubscriptionComponent } from './subscription/subscription.component';
import { AccountComponent } from './account/account.component';
import { ValidateUpdateComponent } from './validate-update/validate-update.component';
import { RedirectComponent } from './redirect/redirect.component';
import { ContactComponent } from './contact/contact.component';
import { RecaptchaService } from './recaptcha.service';
import { TermsAndConditionsComponent } from './terms-and-conditions/terms-and-conditions.component';
import { UnsubscribeComponent } from './unsubscribe/unsubscribe.component';
import { DashboardComponent } from './dashboard/dashboard.component';

@NgModule({
  declarations: [
    AppComponent,
    MainpageComponent,
    CreateAccountComponent,
    ValidationSelectorComponent,
    ValidateAccountComponent,
    ValidatePhoneComponent,
    LoginComponent,
    RecoverSelectorComponent,
    RecoverPasswordComponent,
    ResetPasswordComponent,
    SubscriptionsComponent,
    SubscriptionComponent,
    AccountComponent,
    ValidateUpdateComponent,
    RedirectComponent,
    ContactComponent,
    TermsAndConditionsComponent,
    UnsubscribeComponent,
    DashboardComponent
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    FormsModule,
    ReactiveFormsModule,
    AppRoutingModule,
    RecaptchaModule

  ],
  providers: [
    provideClientHydration(),
    RecaptchaService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
