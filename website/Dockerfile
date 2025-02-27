FROM node:18.19 AS build-angular

WORKDIR /app

COPY package*.json ./
COPY . .

COPY ./generate-environments.sh ./
RUN chmod +x ./generate-environments.sh

RUN ./generate-environments.sh

RUN npm install --legacy-peer-deps
RUN npm run build --configuration=production

FROM nginx:stable-alpine

COPY --from=build-angular /app/dist/website/browser /usr/share/nginx/html

COPY ./replace-env-vars.sh /usr/share/nginx/html/

RUN chmod +x /usr/share/nginx/html/replace-env-vars.sh

ENV API_URL=${API_URL}
ENV POSTGRES_PASSWORD=${POSTGRES_PASSWORD}

EXPOSE 80

CMD ["/bin/sh", "-c", "/usr/share/nginx/html/replace-env-vars.sh && nginx -g 'daemon off;'"]
