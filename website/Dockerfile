FROM node:18.19 AS build-angular

WORKDIR /app

COPY package*.json ./
COPY . .

RUN sed -i 's/\r$//' /app/generate-environments.sh && chmod +x /app/generate-environments.sh

RUN /bin/sh /app/generate-environments.sh

RUN npm install --legacy-peer-deps
RUN npm run build --configuration=production

FROM nginx:stable-alpine

COPY --from=build-angular /app/dist/website/browser /usr/share/nginx/html
COPY ./replace-env-vars.sh /usr/share/nginx/html/

RUN sed -i 's/\r$//' /usr/share/nginx/html/replace-env-vars.sh && chmod +x /usr/share/nginx/html/replace-env-vars.sh

EXPOSE 80

CMD ["/bin/sh", "-c", "/usr/share/nginx/html/replace-env-vars.sh && nginx -g 'daemon off;'"]