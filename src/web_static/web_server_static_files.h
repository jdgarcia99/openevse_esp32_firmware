#include "web_server.apple_touch_icon_png.h"
#include "web_server.assets_charts_CnsZ1jie_css_gz.h"
#include "web_server.assets_charts_D9C7sDdp_js_gz.h"
#include "web_server.assets_en_DaxkwTj__js_gz.h"
#include "web_server.assets_es_dsvpP76Y_js_gz.h"
#include "web_server.assets_fr_jCP6mpT__js_gz.h"
#include "web_server.assets_hu_CbChOcFE_js_gz.h"
#include "web_server.assets_index_BZ8wkAPR_js_gz.h"
#include "web_server.assets_index_CiFvCNic_css_gz.h"
#include "web_server.assets_rolldown_runtime_CbXtAM7H_js_gz.h"
#include "web_server.assets_vendor_D2E1JOXk_js_gz.h"
#include "web_server.favicon_ico.h"
#include "web_server.index_html_gz.h"
#include "web_server.manifest_webmanifest.h"
#include "web_server.pwa_192x192_png.h"
#include "web_server.pwa_512x512_png.h"
#include "web_server.pwa_maskable_512x512_png.h"
#include "web_server.sw_js.h"
StaticFile web_server_static_files[] = {
  { "/apple-touch-icon.png", CONTENT_APPLE_TOUCH_ICON_PNG, sizeof(CONTENT_APPLE_TOUCH_ICON_PNG) - 1, _CONTENT_TYPE_PNG, CONTENT_APPLE_TOUCH_ICON_PNG_ETAG, false },
  { "/assets/charts-CnsZ1jie.css", CONTENT_CHARTS_CNSZ1JIE_CSS_GZ, sizeof(CONTENT_CHARTS_CNSZ1JIE_CSS_GZ) - 1, _CONTENT_TYPE_CSS, CONTENT_CHARTS_CNSZ1JIE_CSS_GZ_ETAG, true },
  { "/assets/charts-D9C7sDdp.js", CONTENT_CHARTS_D9C7SDDP_JS_GZ, sizeof(CONTENT_CHARTS_D9C7SDDP_JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_CHARTS_D9C7SDDP_JS_GZ_ETAG, true },
  { "/assets/en-DaxkwTj_.js", CONTENT_EN_DAXKWTJ__JS_GZ, sizeof(CONTENT_EN_DAXKWTJ__JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_EN_DAXKWTJ__JS_GZ_ETAG, true },
  { "/assets/es-dsvpP76Y.js", CONTENT_ES_DSVPP76Y_JS_GZ, sizeof(CONTENT_ES_DSVPP76Y_JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_ES_DSVPP76Y_JS_GZ_ETAG, true },
  { "/assets/fr-jCP6mpT-.js", CONTENT_FR_JCP6MPT__JS_GZ, sizeof(CONTENT_FR_JCP6MPT__JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_FR_JCP6MPT__JS_GZ_ETAG, true },
  { "/assets/hu-CbChOcFE.js", CONTENT_HU_CBCHOCFE_JS_GZ, sizeof(CONTENT_HU_CBCHOCFE_JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_HU_CBCHOCFE_JS_GZ_ETAG, true },
  { "/assets/index-BZ8wkAPR.js", CONTENT_INDEX_BZ8WKAPR_JS_GZ, sizeof(CONTENT_INDEX_BZ8WKAPR_JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_INDEX_BZ8WKAPR_JS_GZ_ETAG, true },
  { "/assets/index-CiFvCNic.css", CONTENT_INDEX_CIFVCNIC_CSS_GZ, sizeof(CONTENT_INDEX_CIFVCNIC_CSS_GZ) - 1, _CONTENT_TYPE_CSS, CONTENT_INDEX_CIFVCNIC_CSS_GZ_ETAG, true },
  { "/assets/rolldown-runtime-CbXtAM7H.js", CONTENT_ROLLDOWN_RUNTIME_CBXTAM7H_JS_GZ, sizeof(CONTENT_ROLLDOWN_RUNTIME_CBXTAM7H_JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_ROLLDOWN_RUNTIME_CBXTAM7H_JS_GZ_ETAG, true },
  { "/assets/vendor-D2E1JOXk.js", CONTENT_VENDOR_D2E1JOXK_JS_GZ, sizeof(CONTENT_VENDOR_D2E1JOXK_JS_GZ) - 1, _CONTENT_TYPE_JS, CONTENT_VENDOR_D2E1JOXK_JS_GZ_ETAG, true },
  { "/favicon.ico", CONTENT_FAVICON_ICO, sizeof(CONTENT_FAVICON_ICO) - 1, _CONTENT_TYPE_ICO, CONTENT_FAVICON_ICO_ETAG, false },
  { "/index.html", CONTENT_INDEX_HTML_GZ, sizeof(CONTENT_INDEX_HTML_GZ) - 1, _CONTENT_TYPE_HTML, CONTENT_INDEX_HTML_GZ_ETAG, true },
  { "/manifest.webmanifest", CONTENT_MANIFEST_WEBMANIFEST, sizeof(CONTENT_MANIFEST_WEBMANIFEST) - 1, _CONTENT_TYPE_MANIFEST, CONTENT_MANIFEST_WEBMANIFEST_ETAG, false },
  { "/pwa-192x192.png", CONTENT_PWA_192X192_PNG, sizeof(CONTENT_PWA_192X192_PNG) - 1, _CONTENT_TYPE_PNG, CONTENT_PWA_192X192_PNG_ETAG, false },
  { "/pwa-512x512.png", CONTENT_PWA_512X512_PNG, sizeof(CONTENT_PWA_512X512_PNG) - 1, _CONTENT_TYPE_PNG, CONTENT_PWA_512X512_PNG_ETAG, false },
  { "/pwa-maskable-512x512.png", CONTENT_PWA_MASKABLE_512X512_PNG, sizeof(CONTENT_PWA_MASKABLE_512X512_PNG) - 1, _CONTENT_TYPE_PNG, CONTENT_PWA_MASKABLE_512X512_PNG_ETAG, false },
  { "/sw.js", CONTENT_SW_JS, sizeof(CONTENT_SW_JS) - 1, _CONTENT_TYPE_JS, CONTENT_SW_JS_ETAG, false },
};
