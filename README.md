# pathfinder
ARM Hardware Accelerated Pathfinder

## File Organisation (to be refined)

### Hardware

the hardware Verilog code, including associated testbenches and logic Modelsim simulation project

### Quartus

The Quartus Project, mainly relating to synthesis

### Software

All software files, this includes Python prototyping, and `C/C++` code for execution on NIOS.

### Documentation

Set of markdown files providing operating instructions and insight on methods used within the project.
For initial setup, see [setup](./docs/Setup.md).

- [Website](./docs/Website.md)
- [Communication](./docs/Communication.md)
- [Acceleration](./docs/Acceleration.md)

## Commit Messages

Try to use `feat: xxx` where xxx is a description of what the feature is. Try to use imperative tone, for example: "Add a new feature", rather than indicative "Added a new feature". I think the reason for this convention is, amongst many factors, that this fits better when appending commit messages to make a changelog.

There are other variations possible, such as `fix: xxx` or `docs: xxx` which you can use. I do not know much about other message prefixes.

Try to keep commit names short but descriptive so that if something breaks we can easily find which commit to go back to.

During developement you can locally make as many commits as you like, but before pushing, you can do an interactive rebase to squash commits together in to fewer commits. This can help make the working tree cleaner.

Finally, remember to pull before commiting. If you forget and the push is rejected, try `git pull -r` which will rebase the missing commits without creating a spurious branch.

Any questions, please ask me (Diego).
