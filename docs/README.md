# Docs
## Building latex source
1) Setup environment variables for private data: copy the `.env.template`  file
   and update it with the correct data.
   ```bash
   cp report/.env.template report/.env
   ```
2) `cd` to repository root directory and run
   ```bash
   npm run build:report
   ```
   to compile latex files once, or
   ```bash
   npm run watch:report
   ```
   to build latex on file change.
3) Hope you have all the needed dependencies. Otherwise, `tlmgr` is your
   friend :).
4) The pdf file will be generated in the `build` folder.
