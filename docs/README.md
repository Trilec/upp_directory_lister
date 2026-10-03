# Development guidance

Start with the [project README](../README.md) for usage, stack examples, builds
and regression checks. Dated release changes are in [CHANGELOG](../CHANGELOG.md).

This folder contains the U++ guidance retained with the project:

- [General guide](u_general_guide.md)
- [Coding standards](u_coding_standards.md)
- [Anti-bloat review guidance](u_anti-bloat_agent.md)
- [Theme guide](u_theam_guide.md)
- [Ui data-model checklist](UiDataModels_Checklist.md)

Application source is in `DirLister`, screenshots in `design`, and regression
packages in `tests`. Compiler caches and generated test artifacts live outside
the checkout, in the system temporary directory.
