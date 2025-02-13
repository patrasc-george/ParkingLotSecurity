#!/bin/sh

find /usr/share/nginx/html -type f -exec sed -i "s|{{API_URL}}|${API_URL}|g" {} +
find /usr/share/nginx/html -type f -exec sed -i "s|{{POSTGRES_PASSWORD}}|${POSTGRES_PASSWORD}|g" {} +
