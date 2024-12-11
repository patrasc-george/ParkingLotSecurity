import { DOCUMENT } from "@angular/common";
import { Inject, Injectable } from "@angular/core";

declare var grecaptcha: any;

@Injectable({
    providedIn: 'root'
})
export class RecaptchaService {
    private siteKey: string | null = null;
    private loaded: boolean = false;

    constructor(@Inject(DOCUMENT) private readonly document: Document) {
    }

    public load(siteKey: string): void {
        if (this.loaded) {
            return;
        }

        this.siteKey = siteKey;

        const script = document.createElement('script');
        script.src = `https://www.google.com/recaptcha/api.js?render=${siteKey}`;

        this.document.head.appendChild(script);

        this.loaded = true;
    }

    public execute(action: string, callback: (token: string) => void): void {
        if (!this.siteKey) {
            throw new Error('Recaptcha site key is not set.');
        }

        grecaptcha.ready(() => {
            grecaptcha.execute(this.siteKey!, { action })
                .then(callback);
        });
    }
}