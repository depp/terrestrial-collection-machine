def _impl(ctx):
    outputs = []
    for src in ctx.files.srcs:
        name = src.basename
        idx = name.find(".")
        if idx >= 0:
            name = name[:idx]
        out = ctx.actions.declare_file(name + ".html")
        outputs.append(out)
        ctx.actions.run_shell(
            inputs = [src],
            outputs = [out],
            progress_message = "Converting %s to HTML" % src.short_path,
            command = ("pandoc --from=commonmark --to=html5 --standalone " +
                       "--metadata=\"title=$(head -n1 <%s | cut -c3-)\" " +
                       "--output=%s %s") % (src.path, out.path, src.path),
        )
    return [DefaultInfo(files = depset(outputs))]

markdown = rule(
    implementation = _impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
        ),
    },
)
