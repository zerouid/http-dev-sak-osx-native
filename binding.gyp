{
  "targets": [
    {
      "target_name":"launch_priv",
      "sources": ["src/osx_native.cc"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "link_settings": {
        "libraries": [
          "$(SDKROOT)/System/Library/Frameworks/Security.framework",
          "$(SDKROOT)/System/Library/Frameworks/SystemConfiguration.framework",
        ],
      }
    }
  ]
}
