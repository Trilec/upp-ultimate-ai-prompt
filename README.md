# U++ Ultimate AI Prompt

Practical skills for using AI assistants to develop U++ applications and interfaces with the [Ui controls](https://github.com/Trilec/upp_Ui).

This repository has moved from a collection of long session prompts to two focused skills. They include instructions and supporting references covering the U++ details that are easy to get wrong, such as ownership, control lifetimes, layout and styling.

## The two skills

| Skill | What it helps with | Download |
| --- | --- | --- |
| **U++ / Ui development** | Native C++ applications, U++ conventions, memory ownership, controls, layouts, models, themes, PropertyEditor and UMK builds. | [upp-ui-development.zip](skills/upp-ui-development.zip) |
| **Ui HTML mockups** | Polished HTML/CSS/JS prototypes designed to be practically reproduced with native Ui controls and layouts, with a clear handoff to implementation. | [upp-ui-html-mockup.zip](skills/upp-ui-html-mockup.zip) |

Use the development skill for native programming, the mockup skill for browser prototypes, or both when taking a prototype through to a native application.

## Using them

Import the ZIP for each skill into an AI tool that supports skill packages. Each ZIP contains its SKILL.md and supporting files; you do not need to upload those separately. For tools that use skill folders, the unpacked copies are in [skills/](skills/).

Then ask for the skill by name, for example:

> Use upp-ui-development to build a resizable file browser with a tree, table and property inspector, using the Ui controls.

> Use upp-ui-html-mockup to prototype this screenshot. Map the layout and interactions to native Ui controls and identify anything that would need custom implementation.

These are instructions and references for an assistant, not a compiler or a control library. Building a native application still requires U++, the Ui sources and the project's dependencies.

## What else is here?

The [examples/](examples/) folder retains the earlier U++ code examples for reference. They are historical examples, not a claim of validation against the latest Ui APIs. The skills point to current documentation and maintained control examples where appropriate.

## Where the skills are maintained

The main source is [upp_Ui/skills](https://github.com/Trilec/upp_Ui/tree/main/skills), alongside the controls and their documentation. This repository carries distribution copies so people using the original prompt collection can find them here too.

UiDesigner JSON authoring is covered by a separate skill in [upp_uidesigner](https://github.com/Trilec/upp_uidesigner); it is not part of these two packages.
