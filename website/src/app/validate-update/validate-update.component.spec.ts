import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ValidateUpdateComponent } from './validate-update.component';

describe('ValidateUpdateComponent', () => {
  let component: ValidateUpdateComponent;
  let fixture: ComponentFixture<ValidateUpdateComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ValidateUpdateComponent]
    })
      .compileComponents();

    fixture = TestBed.createComponent(ValidateUpdateComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
