import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { MainpageComponent } from './mainpage/mainpage.component';
import { CreateAccountComponent } from './create-subscription/create-subscription.component';
import { ValidateAccountComponent } from './validate-account/validate-account.component';
import { LoginComponent } from './login/login.component';
import { RecoverPasswordComponent } from './recover-password/recover-password.component';
import { ResetPasswordComponent } from './reset-password/reset-password.component';
import { SubscriptionsComponent } from './subscriptions/subscriptions.component';
import { SubscriptionComponent } from './subscription/subscription.component';
import { AccountComponent } from './account/account.component';

const routes: Routes = [
  { path: '', component: MainpageComponent },
  { path: 'create-subscription', component: CreateAccountComponent },
  { path: 'validate', component: ValidateAccountComponent },
  { path: 'login', component: LoginComponent },
  { path: 'recover-password', component: RecoverPasswordComponent },
  { path: 'reset-password', component: ResetPasswordComponent },
  { path: 'subscriptions', component: SubscriptionsComponent },
  { path: 'subscription', component: SubscriptionComponent },
  { path: 'account', component: AccountComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule {

}
