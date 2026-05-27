# Plug-in

#### Contributions

1.  Write feature design specifications using the [community design template](https://gitee.com/opengauss/community/tree/master/design/template) and obtain approval at a Plug-in SIG meeting.
2.  Perform code development and self-testing. Submit code for review and address all review comments.
3.  If documentation changes are required, submit a PR to the [docs repository](https://gitee.com/opengauss/docs). Dolphin-specific syntax documentation can be found [here](https://gitee.com/opengauss/docs/blob/master/content/zh/docs/ExtensionReference/dolphin-Extension.md) (official website link [here](https://docs.opengauss.org/en/docs/latest/extension_reference/dolphin.html)).
4.  Write [check-in specifications](https://gitee.com/opengauss/community/tree/master/check-in/template) using the community check-in template and obtain approval at a Plug-in SIG meeting.
5.  Merge the code.
6.  Write a test report (`openGauss XX Version XX Feature Test Report.md`) using the [QA test report template](https://gitee.com/opengauss/QA/tree/master/Test_Delivery_Templates) and obtain approval at a QA SIG meeting. (Subscribe to the QA [SIG mailing list](https://mailweb.opengauss.org/postorius/lists/qa.opengauss.org/) for meeting schedules).

### Dolphin Plug-in Development Precautions
1.  Check the [plug-in development guide](https://opengauss.org/zh/blogs/ganyang/SQL%E5%BC%95%E6%93%8E%E6%8F%92%E4%BB%B6%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC.html).
2.  If modifications affect [documentation](https://gitee.com/opengauss/docs/tree/master/content/zh/docs/ExtensionReference/dolphin-Extension.md), sync updates to the docs repository. Ensure new SQL syntax includes practical examples.
3.  New or modified code must be wrapped in the `DOLPHIN` macro. This distinguishes plug-in code from kernel code when merging into `openGauss-server`. Use `IF/ELSE` branches to preserve original logic. This applies primarily to `.h` and `.cpp` files; `.y` files can remain unprocessed.
4.  Use the `ENABLE_B_CMPT_MODE` macro for logic involving `dolphin.b_compatibility_mode`.
5.  Verify upgrade and rollback scripts locally. The latest Dolphin version is 3.0. Use the following verification template (replace version numbers as needed):
```
alter system set upgrade_mode to 2;
select pg_sleep(2);
begin;
set isinplaceupgrade to on;
set dolphin.b_compatibility_mode = off;
alter extension dolphin update to '2.0';
alter extension dolphin update to '3.0';
reset dolphin.b_compatibility_mode;
abort;

alter system set upgrade_mode to 0;
```

### Check Case Writing Specifications
1. Check cases default to the `contrib_regression` database (B-compatibility type). No manual database creation is required.
2. Use schemas to isolate results between different test cases. You can refer to the writing method of existing cases.
3. Single case execution should not exceed 10 seconds. Optimize or split cases that exceed this limit.
4. Avoid creating new test groups unnecessarily. A single group should contain 5–10 cases for concurrent execution.
5. Always use an `ORDER BY` clause in `SELECT` statements to ensure stable results.

#### Tips

1.  Use `README_xx.md` for multi-language support, for example, `README_en.md`.
2.  Gitee blog: [blog.gitee.com](https://blog.gitee.com)
3.  Top open-source projects on Gitee: [https://gitee.com/explore](https://gitee.com/explore)
4.  [Gitee Most Valuable Projects (GVP)](https://gitee.com/gvp)
5.  User manual provided by Gitee: [https://gitee.com/help](https://gitee.com/help)
6.  [Gitee Stars](https://gitee.com/gitee-stars/), a column showcasing outstanding Gitee members
