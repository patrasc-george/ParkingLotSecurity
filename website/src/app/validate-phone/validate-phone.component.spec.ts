import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ValidatePhoneComponent } from './validate-phone.component';

describe('ValidatePhoneComponent', () => {
  let component: ValidatePhoneComponent;
  let fixture: ComponentFixture<ValidatePhoneComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ValidatePhoneComponent]
    })
      .compileComponents();

    fixture = TestBed.createComponent(ValidatePhoneComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
