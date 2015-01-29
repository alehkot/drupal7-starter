# FPP Bundles (Fieldable Panels Panes UI)

**Fieldable Panel Panes Bundles (Fieldable Panel Panes UI)** - module that helps to create bundles of the [Fieldable Panels Panes](https://www.drupal.org/project/fieldable_panels_panes) entity. The project aims to simplify developers life by way of refraining of writing the code for panels creation.

The module has an integration with [Features](https://www.drupal.org/project/features) and allows to export an existing bundles with automatically adding their dependencies (fields, field groups, etc).

## Installation

**Drush:**
```
drush en fpp_bundles -y
```

**Manual:**
- Download the release from [drupal.org](https://www.drupal.org/project/fpp_bundles);
- Go to `/admin/modules` and enable the module.

## Changelog

**Version [1.0](https://github.com/BR0kEN-/fpp_bundles/tree/7.x-1.0)**, December 18, 2014:
- UI CRUD interface for bundles of Fieldable Panels Panes entity.
- Integration with Features.

**Version [1.1](https://github.com/BR0kEN-/fpp_bundles/tree/7.x-1.1)**, December 23, 2014:
- Fixed an [issue](https://www.drupal.org/node/2397973) with Features integration.

**Version [1.2](https://github.com/BR0kEN-/fpp_bundles/tree/7.x-1.2)**, December 24, 2014:
- Improved integration with Features. From now, all fields, groups and widgets will be added into the Feature automatically.

**Version [1.3](https://github.com/BR0kEN-/fpp_bundles/tree/7.x-1.3)**, January 29, 2015:
- Added the API hooks which occurs after successful insert/update/delete operation.
- Added the functions for programmatically creation, validation and deletion the bundles.
- Added an option to automatically load CSS & JS for a panel.
- Added the help section at `/admin/help/fpp_bundles`.
- Improved SimpleTest.

## Authors

- [Sergey Bondarenko (BR0kEN)](https://github.com/BR0kEN-)
