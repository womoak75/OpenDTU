/// <reference types="vite/client" />

import { Route, Router } from 'vue-router'
declare module 'vue' {
  interface ComponentCustomProperties {
    $router: Router
    $route: Route
  }
}
