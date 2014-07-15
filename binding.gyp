{
  "targets": [
    {
      "target_name":"launch_priv",
      "sources": ["src/launch_priv_v11.cc"],
      "link_settings": {
        "libraries": [
          "$(SDKROOT)/System/Library/Frameworks/Security.framework",
        ],
      }
    }
  ]
}
