import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RecoverSelectorComponent } from './recover-selector.component';

describe('RecoverSelectorComponent', () => {
  let component: RecoverSelectorComponent;
  let fixture: ComponentFixture<RecoverSelectorComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [RecoverSelectorComponent]
    })
      .compileComponents();

    fixture = TestBed.createComponent(RecoverSelectorComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
