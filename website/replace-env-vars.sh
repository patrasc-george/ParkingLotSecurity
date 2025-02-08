#!/bin/sh

sed -i "s|{{API_URL}}|$API_URL|g" /usr/share/nginx/html/index.html
sed -i "s|{{POSTGRES_PASSWORD}}|$POSTGRES_PASSWORD|g" /usr/share/nginx/html/index.html

nginx -g 'daemon off;'