import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
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
import { TermsAndConditionsComponent } from './terms-and-conditions/terms-and-conditions.component';
import { UnsubscribeComponent } from './unsubscribe/unsubscribe.component';
import { DashboardComponent } from './dashboard/dashboard.component';

const routes: Routes = [
  { path: '', component: MainpageComponent },
  { path: 'create-subscription', component: CreateAccountComponent },
  { path: 'validation-selector', component: ValidationSelectorComponent },
  { path: 'validate', component: ValidateAccountComponent },
  { path: 'validate-phone', component: ValidatePhoneComponent },
  { path: 'login', component: LoginComponent },
  { path: 'recover-selector', component: RecoverSelectorComponent },
  { path: 'recover-password', component: RecoverPasswordComponent },
  { path: 'reset-password', component: ResetPasswordComponent },
  { path: 'subscriptions', component: SubscriptionsComponent },
  { path: 'subscription', component: SubscriptionComponent },
  { path: 'account', component: AccountComponent },
  { path: 'validate-update', component: ValidateUpdateComponent },
  { path: 'redirect', component: RedirectComponent },
  { path: 'contact', component: ContactComponent },
  { path: 'terms-and-conditions', component: TermsAndConditionsComponent },
  { path: 'unsubscribe', component: UnsubscribeComponent },
  { path: 'dashboard', component: DashboardComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule {

}
