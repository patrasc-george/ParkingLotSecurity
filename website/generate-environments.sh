#!/bin/bash

COMPONENTS_DIR="src/app"

ENV_TEMPLATE=$(cat <<EOF
export const environment = {
  API_URL: "{{API_URL}}",
  POSTGRES_PASSWORD: "{{POSTGRES_PASSWORD}}"
};
EOF
)

find "${COMPONENTS_DIR}" -type d | while read -r dir; do
  echo "${ENV_TEMPLATE}" > "${dir}/environment.ts"
  echo "environment.ts file created in ${dir}"
done

echo "All environment.ts files have been generated successfully!"