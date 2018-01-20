vcl 4.0;

import autoparam;

# Default backend definition. Set this to point to your content server.
backend default {
    .host = "127.0.0.1";
    .port = "9000";
}

sub vcl_backend_response {
    if (autoparam.vmod_lru(beresp.url)) {
        set beresp.uncacheable = true;
        set beresp.ttl = 0s;
        return (deliver);
    }
}