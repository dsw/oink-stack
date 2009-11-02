# -*- sh -*-
# see License.txt for copyright and terms of use

#dir $cdir:$cwd:$cdir/../qual/libqual:$cdir/../qual/libregion
# dir $cdir:$cwd:/home/quarl/proj/debfsv/analyzer/oink:/home/quarl/proj/debfsv/analyzer/qual/libqual:/home/quarl/proj/debfsv/analyzer/qual/libregion

#file ./cc_qual
#file ./oink
#file ../elsa/ccparse
#file ./cfgprint
#file ./dfgprint
#file ./rca
#file ./compound_test
#file ./staticprint

#  set args -q-config ../cqual/config/lattice test/template_func6_recursive2.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice examples/foo1.cc
#set args -q-config ../cqual/config/lattice test/template_class3_partial_int.cc.filter-bad.cc
#  set args -q-config ../cqual/config/lattice test/template_class4_func_fwd1.cc.filter-bad.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_recursive2.cc.filter-bad.cc
#set args -q-config ../cqual/config/lattice test/template_func6_recursive2b.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_func6_recursive2b.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_fwd1.cc.filter-bad.cc
#set args -q-config ../cqual/config/lattice test/template_class1.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_class2.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice -tr topform test/template_class2.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_func3_partial.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_func6_fwd1.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_func6_fwd1.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func2_multiple.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_fwd1.cc.filter-good.cc
# set args -tr topform -q-config ../cqual/config/lattice test/template_func3_partial.cc.filter-bad.cc
#set args -tr template -tr odr -tr topform -q-config ../cqual/config/lattice test/template_func6_fwd1.cc.filter-bad.cc
#set args -tr odr -tr template -q-config ../cqual/config/lattice examples/foundScope2.cc
#set args -tr odr -tr template -q-config ../cqual/config/lattice test/template_func6_recursive4.cc.filter-bad.cc
#set args -tr odr -tr template -tr topform -q-config ../cqual/config/lattice test/template_class4_func_fwd1.cc.filter-bad.cc
#set args -tr topform -tr template -tr odr -q-config ../cqual/config/lattice test/template_func6_defafteruse2.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func2.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_defafteruse2.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_defafteruse3.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_defafteruse3.cc.filter-bad.cc
#set args -q-config ../cqual/config/lattice test/template_class4_func_fwd1.cc.filter-bad.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_class4_func_fwd1.cc.filter-bad.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_class1.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_defafteruse2.cc.filter-bad.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func6_fwd1.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func2_multiple.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func1.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_class4_func_fwd1.cc.filter-bad.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func1.cc.filter-good.cc
#  set args -tr topform -q-config ../cqual/config/lattice test/template_class4_func_fwd4.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_func7_oload1.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_class3_partial_int.cc.filter-good.cc
#set args -tr topform -q-config ../cqual/config/lattice test/memberInit_cdtor1.cc.filter-good.cc
#set args -tr topform ../elsa/in/t0010.cc
#set args -tr topform -q-config ../cqual/config/lattice test/template_class4_func_fwd2.cc.filter-bad.cc
#set args -tr topform -q-config ../cqual/config/lattice test/non-compound_IN_ctor3.cc
#set args ../elsa/in/t0017.cc
#set args -q-config ../cqual/config/lattice test/template_class1.cc.filter-bad.cc
#set args -fo-lang kandrC -fq-no-exit-at-inconsistency -fq-no-nonfunky-leaf-polymorphism -fo-no-print-startstop -q-config ../cqual/config/lattice -fprint-results test2-cc/user.c
#set args -tr topform -q-config ../cqual/config/lattice test/template_func7_oload5.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/template_class4_func_fwd3.cc.filter-good.cc
#  set args -q-config ../cqual/config/lattice test/template_class2_forward1.cc.filter-good.cc
#set args -tr nohashline -fo-lang kandrC /home/dsw/oink_extra/bad/stmt_expr.i
#set args -tr nohashline -fo-lang kandrC /home/dsw/oink_extra/bad/call_undeclared_func.i
#set args -fo-lang kandrC -fo-exit-after-typecheck -fq-no-inference ../cqual/tests/linux/rtc.i.out
#set args -tr nohashline -fo-lang kandrC /home/dsw/oink_extra/bad/kandr_empty_args_def.i
#set args test/cond_omit_then.cc
#set args -fm-print-cfg if_stmt.i
#set args -q-config ../cqual/config/lattice test/nested_func1.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/nested_func1.cc.filter-bad.cc
#  set args /home/dsw/oink_extra/bad/oper_ambig1.cc
#  set args -tr nohashline -fo-lang kandrC /home/dsw/oink_extra/bad/kandr_func.c
#  set args -fo-print-ast -fo-exit-after-parse -tr nohashline -fo-lang kandrC /home/dsw/oink_extra/bad/kandr_func.c
#  set args -tr nohashline -fo-lang kandrC /home/dsw/oink_extra/bad/const_eval.cc
#set args -tr nohashline /home/dsw/oink_extra/bad/delta5/file.cc
#  set args /home/dsw/oink_all/elsa/in/big/nsHTMLEditRules.i
#  set args -tr nohashline -fo-lang kandrC /home/ballA/amanda-2.4.3-4/amcheck-hUQ3.i
#set args -tr nohashline -fo-print-typed-ast -fo-lang kandrC /home/dsw/oink_all/oink/examples/kandr1.c
#set args -tr nohashline /home/ballA/libwvstreams-3.70-8/cfgentry-aklT.i
#set args -fo-lang kandrC -tr nohashline /home/ballA/doxygen-1.2.18-3/png-jmoU.i
#set args -fo-lang kandrC -tr nohashline /home/dsw/oink_extra/bad/impl_array_size.c
#set args -fo-lang kandrC -fo-print-typed-ast /home/dsw/oink_all/oink/test/impl_int.c
#  set args ../elsa/in/t0003.cc
#  set args -fo-lang kandrC -tr disamb -fo-print-ast examples/ambig1.c
#  set args -tr disamb -fo-print-ast examples/ambig1.c
#set args -tr printAST ../elsa/in/t0003.cc
#set args -tr disamb -fo-print-ast -fo-lang kandrC test/gcc_assert_fail.i
#  set args -tr disamb -fo-print-ast -fo-print-typed-ast -fo-lang kandrC /home/dsw/oink_all/oink/test/impl_int.c
#  set args -tr disamb -fo-print-ast -fo-print-typed-ast -fo-lang kandrC /home/dsw/oink_all/oink/examples/delta1/rtc.c
#set args -tr disamb -fo-print-ast -fo-print-typed-ast -fo-lang kandrC /home/dsw/oink_all/oink/test/impl_int3.c
#set args -fo-lang kandrC test/gcc_pretty.i
#set args -fo-print-ast -fo-lang kandrC examples/kandr_empty_args_def.i
#  set args -fo-print-ast -fo-lang kandrC examples/va_end.c
#  set args -fo-print-ast -fo-lang kandrC examples/reg_param.c
#  set args -fo-print-ast -fo-lang kandrC test/impl_int2.c
#  set args -fo-lang kandrC -tr cancel,action,parse examples/delta2/impl_int2.c
#set args -fo-exit-after-parse -fo-lang kandrC test/kandrfunc1.c
# REDO THIS (as cc_qual)
#  set args -fo-lang kandrC -fo-exit-after-typecheck -fq-no-inference ../cqual/tests/linux/rtc.i.out
#  set args -tr topform -fo-lang kandrC test/impl_int4.c
#set args -tr topform -fo-print-ast -fo-lang kandrC examples/ptrarr.c
#  set args -fo-print-typed-ast test/nosize_array_init.c
#  set args -fo-print-typed-ast examples/arr1.c
#set args -fo-print-typed-ast examples/enum_sup.cc
#  file ./compound_test
#  set args ../compound_init/test/compoundinit1A.c
#set args -fo-lang kandrC -fo-exit-after-typecheck -fq-no-inference ../cqual/tests/linux/rtc.i.out
#  set args -fo-lang kandrC -fo-print-ast -fq-no-exit-at-inconsistency -fq-no-nonfunky-leaf-polymorphism -fo-no-print-startstop -q-config ../cqual/config/lattice -fprint-results ../cqual/tests/multidim.c
#  set args /home/ballAruns/run17/out/arts-1.1-7/artscat-dxya.i
#  set args /home/ballA/arts-1.1-7/artscat-dxya.i
#set args /home/dsw/ballAruns/artscat-dxya.deattr.i
#set args -q-config ../cqual/config/lattice test/non-compound_IN_ctor1.cc
#set args -q-config ../cqual/config/lattice test/tb2050funky.c
#  file ./dfgprint
#  set args -fm-print-dfg test/hello.cc
#set args -fm-print-dfg examples/a.cc
#  set args -fm-print-dfg test/hello.cc
#  set args -fm-print-dfg examples/call.cc
#file ./oink
#  set args -fo-print-typed-ast test/inline_asm1.cc
#set args test/inline_asm1.cc
#  file ./compound_test
#  set args ../compound_init/test/compoundinit1A.c
#file ./cc_qual
#set args -fq-no-inference test/parse_mess1.c
#set args -fq-no-inference test/parse_mess1.c
#set args -q-config ../cqual/config/lattice test/tb2050funky.c
#  file ./dfgprint
#  set args -fm-no-print-dfg test/inline_asm1.cc
#  set args -fq-no-inference test/parse_mess1.c
#set args examples/rca1.cc
#  file ./cfgprint
#  set args -fc-print-cfg test/hello.cc
#  file ./dfgprint
#  set args -fd-print-dfg test/hello.cc
#  file ./cc_qual
#  set args -q-config ../cqual/config/lattice test/non-compound_IN_ctor1.cc
#set args -fo-print-typed-ast test/nosize_array_init.c
#  set args -fq-no-inference test/parse_mess1.c
#  file ./staticprint
#  set args -fs-print-ihg test/hello.cc
#set args -q-config ../cqual/config/lattice test/method_call12a.cc test/method_call12b.cc; test $? -eq 32
#set args -q-config ../cqual/config/lattice -fo-lang kandrC test/c_linkage1.c test/c_linkage2.c
#  set args -fo-lang kandrC -q-config ../cqual/config/lattice -fq-no-exit-at-inconsistency test/c_linkage1.c test/c_linkage2.c
#set args -fo-lang kandrC -tr nohashline /home/ballBruns/examples/11.i
#set args -tr nohashline -fo-lang gnu2_kandrC /home/ballBruns/examples/20.i
#set args -tr nohashline /disk2/home/dsw/delta10/BreakPM-crjq.ii
#set args -q-config ../cqual/config/lattice test/immediate_cdtor2.cc.filter-good.cc
#  set args /disk2/home/dsw/delta11/TransformFOTBuilder-WKOy.ii
#set args /disk2/home/dsw/delta15/AdditiveExpr-rZa0.ii
#  set args /disk2/home/dsw/delta16/diagnose-0RQi.ii
#set args -tr nohashline /disk2/home/ballB/jikes-1.15-1/double-2unl.ii
#set args -tr nohashline /disk2/home/dsw/delta17/in.ii
#set args ../compound_init/test/compoundinit6B.c
#  set args /disk2/home/dsw/delta18/RefreshDI-aWbT.ii
#  set args /home/ballBruns/delta3/nsAString-9aov.ii
#set args /home/ballBruns/delta4/nsDOMClassInfo-5hMt.ii
#set args -fo-lang ansiC -fo-print-typed-ast test/nosize_array_init.c
#set args -tr matchtype /home/ballBruns/delta5/nsLinebreakConverter-Bcjv.ii
#set args test/error_var1.cc
#  set args ../elsa/in/t0114.cc
#set args /home/ballBruns/delta6/address-IZz4.ii
#set args -tr topform /home/ballBruns/delta7/CNavDTD-d8S0.ii
#set args /home/ballC/Glide3-20010520-13/ccRjfBqE-13545.i
#set args /home/ballC/Glide3-20010520-13/ccHa3zY9-13705.i
#set args  -tr nohashline -fo-lang gnu2_kandrC /disk2/home/dsw/deltaC1/ccRjfBqE-13545.i
#set args -fo-lang ansiC  -fo-print-typed-ast test/nosize_array_init.c
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -tr nohashline -fo-lang gnu2_kandrC /disk2/home/ballC/kernel-2.4.18-3/STDIN-6812.g.i
#set args -fo-lang ansiC /home/dsw/oink_all/compound_init/test/compoundinit7_e_cpdlit.c
#set args -fo-lang ansiC ../compound_init/test/compoundinit7_e_cpdlit.c
#set args -tr nohashline -fo-lang gnu2_kandrC /home/dsw/c/cpd2.c
#set args -tr nohashline -fo-lang gnu2_kandrC /home/dsw/oink_all/compound_init/test/compoundinit9_chararr.c
#set args -tr nohashline -fo-lang gnu2_kandrC /home/dsw/oink_all/compound_init/test/compoundinit8_chararraymem.c
#set args -fo-lang ansiC ../compound_init/test/compoundinit1G.c
#set args -fo-lang ansiC -q-config ../cqual/config/lattice -fq-no-exit-at-inconsistency test/field2.c
#set args  -fo-lang suffix test/bool.ii
#set args -tr nohashline  -fo-lang suffix  -program-files /home/dsw/ball/./mozilla-0.9.9-7/libgfx_gtk.so.ld
#set args -tr nohashline -fo-lang suffix /disk2/home/ballC/mozilla-0.9.9-7/STDIN-12989.g.ii
#  set args -tr nohashline -fo-lang suffix -q-config  ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-10164.g.i
#set args -tr nohashline test/void_star_func.i
#  set args -tr nohashline -fo-lang suffix -q-config  ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-18054.g.i
#set args -tr nohashline -fo-lang suffix -q-config  ../cqual/config/lattice test/a.i
#set args -tr nohashline -fo-lang suffix -q-config  ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-10418.g.i
#set args -tr nohashline -fo-lang suffix -q-config  ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-23161.g.i
#set args -fo-lang suffix  -q-config ../cqual/config/lattice test/varargsE.i
#set args -fo-lang suffix -tr nohashline -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-10232.g.i
#set args -fo-lang suffix -tr nohashline -q-config ../cqual/config/lattice test/void_star_to_double_ptr.i
#set args -fo-lang suffix -tr nohashline -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-10405.g.i
#set args -fo-lang suffix -tr nohashline -q-config ../cqual/config/lattice test/cast_assign.i
#set args -fo-lang suffix -tr nohashline -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-11780.g.i
#set args -q-config ../cqual/config/lattice test/stmt_expr_no_edge.i
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-11741.g.i
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-10405.g.i
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/string_lit_nonconst.i
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-29344.g.i
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-12447.g.i
#  set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-24897.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice test/method1.cc
#  set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-24897.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice test/new1.cc
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-9712.g.i
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-10000.g.ii
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-26308.g.i
#  set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-28480.g.i
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice test/char_const1.i
#  set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-10232.g.i
#  set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice test/cpd_lit_arg.cc
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice test/inc_ref.cc
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-1142.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-16523.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-14570.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-14570.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/runs/delta9/STDIN-14570.g.ii
#set args -tr nohashline /home/dsw/runs/delta9/STDIN-14570.g.ii
#  set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/mozilla-0.9.9-7/STDIN-5239.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice test/grouping_fun.c
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-11119.g.ii
#set args -tr nohashline -fo-lang cc -q-config ../cqual/config/lattice test/def_arg_cast.ii
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./kernel-2.4.18-3/STDIN-15598.g.i
#  set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/ball/./mozilla-0.9.9-7/STDIN-18543.g.ii
#set args -tr nohashline -fo-lang suffix -q-config ../cqual/config/lattice /home/dsw/runs/delta13/STDIN-18543.g.ii
#set args -tr nohashline -q-config ../cqual/config/lattice test/heap_cdtor2.cc.filter-bad.cc
#set args -tr nohashline -q-config ../cqual/config/lattice test/new1.cc
#set args -tr parseTree -fo-lang
#set args -fo-lang kandrC -fq-no-exit-at-inconsistency -fq-no-nonfunky-leaf-polymorphism -fo-no-print-startstop -q-config ../cqual/config/lattice -fprint-results test2-cc/crazy_struct.c
#set args -q-config ../cqual/config/lattice -fq-no-exit-at-inconsistency -fprint-results -hotspots 10 test/tb2000fail.c
#set args -q-config ../cqual/config/lattice -fprint-results test/tb2000fail.c
#set args -fq-no-inference test/parse_mess1.c
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.c.out1
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -fq-no-inference test/parse_mess1.c
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -fq-no-inference test/parse_mess1.c
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -fs-print-ihg test/hello.cc test/hello2.cc
#set args -q-config ../cqual/config/lattice test/field_sensitive2a.cc.filter-good.cc
#set args -fd-print-dfg test/hello.cc
#set args ../elsa/in/t0013.cc
#set args -fo-print-typed-ast -fo-print-ML-types ../elsa/tmp/t0013b.cc
#set args -fo-no-check-AST-integrity -fo-print-typed-ast -fo-print-ML-types ../elsa/tmp/t0013b.cc
#set args -fd-print-dfg test/hello.cc
#set args -fd-print-dfg test/hello.cc
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -fd-no-print-dfg test/inline_asm1.cc
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/char_array_lit_init2.i
#set args -fd-print-dfg test/hello.cc
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -q-config ../cqual/config/lattice test/non-compound_IN_ctor2.cc
#set args -q-config ../cqual/config/lattice test/non-compound_IN_ctor2.cc
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/char_array_lit_init2.i
#set args -fd-print-dfg test/hello.cc
#set args -q-config ../cqual/config/lattice test/ptr-to-member3d.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice -fcasts-preserve test/casts_preserve1.c
#set args -q-config ../cqual/config/lattice -fcasts-preserve test/casts_preserve2.c
#./cc_qual -fcasts-preserve -fo-lang cc -q-config ../cqual/config/lattice -fprint-quals-graph -fugly -fq-no-exit-at-inconsistency test/t1330_cast.c.filter-good.cc
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice  -fugly test/field_sensitive0.cc.filter-bad.cc
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.c.out1
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/char_array_lit_init.i
#  set args -q-config ../cqual/config/lattice test/heap_cdtor3.cc.filter-bad.cc
#  set args -fq-no-inference test/heap_cdtor3.cc.filter-bad.cc
#set args -tr printTypedAST,prettyPrint a.cc
#set args -fq-no-inference a.cc
#set args -fq-no-inference -fo-pretty-print a.cc
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.c.out1
#set args -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.c
#set args -q-config ../cqual/config/lattice -fcasts-preserve -fq-casts-preserve-below-functions test/casts_preserve_func1.c
#set args -q-config ../cqual/config/lattice test/heap_cdtor3.cc.filter-bad.cc
#set args -q-config ../cqual/config/lattice -fo-lang gnu2_kandrC test/field_sensitive2a.cc.filter-bad.cc
#set args -fo-lang  gnu2_kandrC -q-config ../cqual/config/lattice -fq-no-exit-at-inconsistency test/field_sensitive2a.cc.filter-bad.cc
#set args test/atom_type_tag.cc
#set args -q-config ../cqual/config/lattice test/ptr-to-member1.cc.filter-good.cc
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/field_sensitive2b.cc.filter-bad.cc
#set args -fq-no-inference test/parse_mess1.c
#set args -fq-no-inference parse_mess1.c
#set args -q-config ../cqual/config/lattice test/func3.c
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/void_star_poly2.cc.filter-good.cc
#set args -fq-no-inference test/parse_mess2.c
#set args -fo-lang gnu2_kandrC -q-config ../cqual/config/lattice test/int_as_void_star_poly1.cc.filter-bad.cc
#set args -fo-lang cc -q-config /home/dsw/oink_all/cqual/config/lattice -fo-pretty-print -fq-print-trans-qual matt1.cc
#set args -fo-lang gnu2_kandrC -q-config lattice2 test/int_as_void_star_poly4a.cc.filter-bad.cc
#set args test/atom_type_tag.cc
#set args test/atom_type_tag.cc
#set args -fo-pretty-print ../elsa/in/t0033.cc
#set args -fo-pretty-print ../elsa/in/t0019.cc
#set args -q-config ../cqual/config/lattice test/template_class3_partial_int.cc.filter-bad.cc
#set args -q-config ../cqual/config/lattice -fcasts-preserve -fo-lang ansiC -fo-pretty-print -fq-print-trans-qual bftpd.c
#set args -q-config ../cqual/config/lattice -fcasts-preserve -fo-lang ansiC -fo-pretty-print test/cpd_init1.c
#set args -q-config ../cqual/config/lattice -fo-instance-sensitive -fo-lang ansiC test/cpd_init1.c
#set args -q-config ../cqual/config/lattice test/template_class3_partial_int.cc.filter-bad.cc
#set arg test/error_var1.cc
#set args -q-config ../cqual/config/lattice test/memberInit_cdtor1.cc.filter-good.cc
#set args -q-config ../cqual/config/lattice test/heap_cdtor3.cc.filter-bad.cc
#set args -fo-lang cc -q-config ../libqual/config/lattice -fcasts-preserve test/casts_preserve2.c
#set args -fo-lang kandrC -q-config ../libqual/config/lattice -fcasts-preserve test/casts_preserve2.c
#  set args -fd-print-dfg test/hello.cc
#set args -fd-print-dfg test/foo1.cc
#set args -fo-lang ansiC test/nosize_array_init.c
#set args ../elsa/in/d0005.cc
#set args -q-config ../libqual/config/lattice test/ellipsis3.c
#set args  -fo-report-unsatisfied-links test/foo1.c
#set args -fo-lang ansiC ../compound_init/test/compoundinit5A.c
#set args -fo-lang gnu2_kandrC -q-config ../libqual/config/lattice test/char_array_lit_init.i
#set args -fo-pretty-print ../elsa/in/t0026.cc
#set args -fq-no-inference -fo-pretty-print test/parse_mess1.cc
#set args -fo-pretty-print ../elsa/in/t0001.cc
#set args  -fo-report-unsatisfied-symbols test/unsat_symbol1.c.filter-bad.c
#set args test/atom_type_tag.cc
#set args test/asm.i
#set args -fo-lang ansiC -fo-print-typed-ast test/nosize_array_init.c
#set args -fq-no-inference test/parse_mess1.cc
#set args test/double_goto.i
#set args test/atom_type_tag.cc
#set args -fo-lang ansiC -fo-print-typed-ast test/nosize_array_init.c
#set args test/atom_type_tag.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fd-print-dfg test/hello.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fo-lang cc -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.cc.out1
#set args -fo-lang cc -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -q-config ../libqual/config/lattice test/abstract-func.cc
#set args -q-config ../libqual/config/lattice test/method_call3g.cc
#set args -q-config ../libqual/config/lattice test/array_pointer1.c
#set args -q-config ../libqual/config/lattice test/func1.c
#set args -fs-print-ihg test/hello.cc test/hello2.cc
#set args -q-config ../libqual/config/lattice test/ssh1.c
#set args -q-config ../libqual/config/lattice test/varargs1.c
#set args -q-config ../libqual/config/lattice test/array_pointer1.c
#set args -fd-print-dfg test/hello.cc
#set args -q-config ../libqual/config/lattice test/method_call13.cc
#set args -fo-lang gnu2_kandrC -q-config ../libqual/config/lattice test/stmt_expr_no_edge.i
#set args -q-config ../libqual/config/lattice test/non-compound_IN_ctor1.cc
#set args -q-config ../libqual/config/lattice test/non-compound_IN_ctor3.cc
#set args -q-config ../libqual/config/lattice test/non-compound_IN_ctor3.cc
#set args -q-config ../libqual/config/lattice test/varargs1.c
#set args -q-config ../libqual/config/lattice test/method_call1.cc
#set args -q-config ../libqual/config/lattice test/method_call1.cc
#set args -fo-lang cc -q-config ../libqual/config/lattice -fprint-quals-graph -fugly -fq-no-exit-at-inconsistency test/method_call1.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -q-config ../libqual/config/lattice test/array_pointer1.c
#set args -q-config ../libqual/config/lattice test/funcall2.c
#set args -q-config ../libqual/config/lattice test/method_call3g.cc
#set args -q-config ../libqual/config/lattice test/const1.c
#set args -q-config ../libqual/config/lattice test/abstract-func.cc
#set args -q-config ../libqual/config/lattice test/simple5.c
#set args -q-config ../libqual/config/lattice test/inc_ref.cc
#set args -q-config ../libqual/config/lattice test/exception1b.cc.filter-good.cc
#set args -fo-lang gnu2_kandrC -fo-instance-sensitive -q-config ../libqual/config/lattice test/field_sensitive0.cc.filter-good.cc
#set args -q-config ../libqual/config/lattice test/exception1b.cc.filter-bad.cc
#set args -q-config ../libqual/config/lattice test/ptr-to-member2.cc.filter-bad.cc
#set args -fo-lang gnu2_kandrC -fo-instance-sensitive -q-config ../libqual/config/lattice test/field_sensitive0.cc.filter-bad.cc
#set args -q-config ../libqual/config/lattice -fq-no-exit-at-inconsistency -fprint-results -hotspots 10 test/tb2000fail.c
#set args -q-config ../libqual/config/lattice -fo-lang ansiC test/cpd_init1.c
#set args -q-config ../libqual/config/lattice test/multi1_a.c test/multi1_b.c
#set args test/double_goto.i
#set args test/atom_type_tag.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -q-config ../libqual/config/lattice test/cpd_lit_arg.cc
#set args -q-config ../libqual/config/lattice test/tg2060leafpoly.c
#set args -q-config ../libqual/config/lattice -fo-lang ansiC test/cpd_init1.c
#set args -q-config ../libqual/config/lattice umesh2.c
#set args -q-config ../libqual/config/lattice umesh1.c
#set args -fq-no-inference test/parse_mess1.cc
#set args -fo-lang gnu2_kandrC -fo-instance-sensitive -q-config ../libqual/config/lattice test/field_sensitive0.cc.filter-good.cc
#set args -fo-lang ansiC -fo-print-typed-ast test/nosize_array_init.c
#set args -q-config ../libqual/config/lattice test/nested_func1.cc.filter-bad.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fq-no-inference -fo-pretty-print test/parse_mess1.cc
#set args -fo-pretty-print ../elsa/in/t0004.cc
#set args -fo-pretty-print ../elsa/in/t0026.cc
#set args -fo-pretty-print ../elsa/in/t0035.cc
#set args -fo-pretty-print ../elsa/in/t0120.cc
#set args -fo-pretty-print ../elsa/in/t0026.cc
#set args -fo-pretty-print ../elsa/in/t0168.cc
#set args -fo-pretty-print ../elsa/in/t0168.cc
#set args -fo-pretty-print ../elsa/in/t0254.cc
#set args -fd-print-dfg test/hello.cc
#set args -fo-lang cc -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.cc.out1
#set args -fo-lang cc -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/assert_type1.c
#set args -fo-no-do-elaboration -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/tb2000fail.c
#set args -q-config ../libqual/config/lattice test/method_call3g.cc
#set args -q-config ../libqual/config/lattice test/func1.c
#set args -q-config ../libqual/config/lattice test/void_nonfs1.cc
#set args -q-config ../libqual/config/lattice test/funcall2.c
#set args -q-config ../libqual/config/lattice test/void_nonfs1.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -q-config ../libqual/config/lattice test/abstract-func.cc
#set args -q-config ../libqual/config/lattice test/abstract-func.cc
#set args -q-config ../libqual/config/lattice test/method_call3g.cc
#set args -fo-lang cc -q-config ../libqual/config/lattice -fo-pretty-print -fq-print-trans-qual test/print_trans_qual1.cc
#set args -fo-lang gnu2_kandrC -fo-instance-sensitive -q-config ../libqual/config/lattice test/extern_inline_func1.c.filter-bad.cc
#  set args -fq-name-with-loc -q-config ../libqual/config/lattice test/multi5_a.c test/multi5_b.c
#set args -q-config ../libqual/config/lattice test/field_sensitive2c.cc.filter-bad.cc
#set args -fo-lang cc -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.cc.out1
#set args -fq-name-with-loc -fprint-quals-graph -q-config ../libqual/config/lattice -prelude test/prelude_funky1.c test/prelude_funky1main.c
#set args -fq-no-inference test/parse_mess1.cc
#set args -fpoly -q-config ../libqual/config/lattice test/poly_global2.cc.filter-bad.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -q-config ../libqual/config/lattice test/funcall2.c
#set args -q-config ../libqual/config/lattice test/tb2050funky.c
#set args -q-config ../libqual/config/lattice test/funky5.cc.filter-good.cc
#set args -q-config ../libqual/config/lattice -prelude test/prelude_p1.c test/prelude_main1_mod.c
#set args -q-config ../libqual/config/lattice test/tb2050funky.c
#set args -fq-no-inference test/parse_mess1.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fq-no-inference -fo-pretty-print test/parse_mess1.cc
#set args -fq-no-nonfunky-leaf-polymorphism -q-config ../libqual/config/lattice test/tg2060leafpoly.c
#set args -q-config ../libqual/config/lattice -prelude test/prelude_p1.c test/prelude_main1_mod.c
#set args -fo-no-do-elaboration -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/struct_sizeof.c
#set args retVar1.cc
#set args test/atom_type_tag.cc
#set args -fd-print-dfg retVar1.cc
#set args -fd-print-dfg test/hello.cc
#set args -fq-no-inference test/parse_mess1.cc
#set args -fo-lang ansiC -fo-print-typed-ast test/nosize_array_init.c
#set args -fo-lang gnu2_kandrC -q-config ../libqual/config/lattice test/varargsE.i
#set args test/double_goto.i
#set args -fo-lang ansiC -fo-print-typed-ast test/nosize_array_init.c
#set args test/const_array.cc
#set args -fo-pretty-print ../elsa/in/t0003.cc
#set args -fo-pretty-print ../elsa/in/t0026.cc
#set args -fo-pretty-print ../elsa/in/t0001.cc
#set args -fo-pretty-print ../elsa/in/t0026.cc
#set args -fd-print-dfg hello.cc
#set args -fq-no-inference qualifiers2.cc
#set args -fq-name-with-loc -fpoly -q-config ../libqual/config/lattice -fprint-quals-graph -fugly -fq-no-exit-at-inconsistency test/non-compound_IN_ctor1.cc
#set args -q-config ../libqual/config/lattice test/non-compound_IN_ctor1.cc
#set args -fo-lang cc -fq-no-inference -fo-pretty-print -fo-no-print-startstop test/parse_mess1.cc.out1
#set args -fo-linker-vis-to-xml examples/rupak1.cc
#set args -fo-linker-vis-to-xml test/hello.cc
#  set args BacktrackingFlowGraph_sf.cc

# check this one for struct field sensitivity
#set args -fo-lang gnu2_kandrC -fo-instance-sensitive -q-config ../libqual/config/lattice test/field_sensitive0.cc.filter-bad.cc

# check this one for void * poly names
#set args -fo-lang gnu2_kandrC -fo-instance-sensitive -q-config ../libqual/config/lattice test/extern_inline_func1.c.filter-bad.cc

#file ./oink
#  set args tmp3.ii
#set args test/atom_type_tag.cc
#set args test/error_var1.cc
#set args ../elsa/in/t0279.cc
#set args -fo-report-unsatisfied-symbols test/unsat_symbol1.c.filter-bad.c 2>&1
#  file ./dfgprint
#  set args -fd-print-dfg test/hello.cc

#file ./cc_qual
#set args -q-config ../libqual/config/lattice test/taint1.c -serialize-qual test/taint1.qdir
# set args -fq-link -q-config ../libqual/config/lattice test/taint1.qdir test/taintB.qdir
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -serialize-qual test/taint1.c.2.qdir
#set args -q-config ../libqual/config/lattice test/taint1.c -serialize-qual test/taint1.c.1.qdir
#set args -q-config ../libqual/config/lattice test/taint1.c -serialize-qual test/taint1.c.1.qdir
#set args -q-config ../libqual/config/lattice test/a.c -serialize-qual test/foo.qdir
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -serialize-qual test/taint1.c.2.qdir
#set args -q-config ../libqual/config/lattice test/taint1.c -serialize-qual test/taint1.c.1.qdir
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -serialize-qual test/taint1.c.2.qdir
#set args -q-config ../libqual/config/lattice test/allvalues.cc -serialize-qual test/allvalues.cc.1.qdir

# try just a C file
#set args -q-config ../libqual/config/lattice test/simple_struct.c -serialize-qual test/allvalues.cc.1.qdir

#set args -q-config ../libqual/config/lattice test/allvalues.cc -serialize-qual test/allvalues.cc.1.qdir
#set args -q-config ../libqual/config/lattice test/multi2_a.c test/multi2_b.c
#  file ./oink
#  set args test/atom_type_tag.cc

#file ./cc_qual
#set args -q-config ../libqual/config/lattice test/template_func2.cc.filter-bad.cc
#  set args -q-config ../libqual/config/lattice test/multi4_a.c test/multi4_b.c
#set args -q-config ../libqual/config/lattice test/multi4_b.c
#set args -fq-link -q-config ../libqual/config/lattice test/taint2_a.c.a.qdir test/taint2_b.c.b.qdir -fprint-quals-graph

#set args -fq-link -q-config ../libqual/config/lattice test/taint2_a.c.a.qdir test/taint2_b.c.b.qdir
#set args -q-config ../libqual/config/lattice -fq-link test/taintA.c.qdir
#set args -q-config ../libqual/config/lattice -fq-link test/taintA.c.qdir -fprint-quals-graph
#set args -q-config ../libqual/config/lattice -fq-link test/taint2_a.c.qdir test/taint2_b.c.qdir
#set args -fpoly -q-config ../libqual/config/lattice -fq-link test/taint2_a.c.qdir test/taint2_b.c.qdir -fprint-quals-graph
#set args -fpoly -q-config ../libqual/config/lattice test/taint2_a.c -fq-no-exit-at-inconsistency -fq-name-with-loc -fq-name-with-serialno -fugly -serialize-qual test/taint2_a.c.qdir
#set args -q-config ../libqual/config/lattice test/nested_func2.cc.filter-good.cc
#set args -q-config ../libqual/config/lattice test/method_call10a.cc test/method_call10b.cc; test $? -eq 32
#set args -q-config ../libqual/config/lattice -control test/simple1.ctl test/taint1.c
#set args -q-config ../libqual/config/lattice test/t1352_ref.cc.filter-good.cc
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol3b.qdir test/unsat_symbol3b.qdir
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol3b.qdir test/unsat_symbol3c.qdir
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/completely_empty.qdir
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol1.c.filter-good.qdir
#  file ./oink
#  set args test/double_goto.i
#file ./cc_qual
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -serialize-qual test/taint1.c.2.qdir
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol3b.qdir test/unsat_symbol3c.qdir -control test/unsat_symbol3c.ctl
#  file ./dfgprint
#  set args -fd-print-dfg test/hello.cc
# file ./cc_qual
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol3b.qdir test/unsat_symbol3c.qdir -control test/unsat_symbol3c2.ctl
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol3d.qdir -control test/unsat_symbol3c3.ctl
#set args -q-config ../libqual/config/lattice test/unsat_symbol3d.c -serialize-qual test/unsat_symbol3d.qdir  -control test/unsat_symbol3c3.ctl
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol5b.qdir
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol5b.qdir test/unsat_symbol5c.qdir -control test/unsat_symbol5c.ctl
#set args -q-config ../libqual/config/lattice -fq-link -fo-report-unsatisfied-symbols test/unsat_symbol5b.qdir test/unsat_symbol5c.qdir -control test/unsat_symbol5c.ctl

# set args -q-config ../libqual/config/lattice test/prelude_A1.c
#  set args -q-config ../libqual/config/lattice -fq-link -fpoly -control test/prelude_A4b.ctl test/prelude_A4a.qdir test/prelude_A4b_nof.qdir
#  file ./oink
#  set args ../elsa/in/t0019.cc
#  file ./cc_qual
#  set args -q-config ../libqual/config/lattice test/method_call5.cc.filter-good.cc
#  file ./dfgprint
#  set args -fd-print-dfg test/hello.cc

#  file ./cc_qual
#set args -q-config ../libqual/config/lattice test/method_call5.cc.filter-good.cc
# set args -q-config ../libqual/config/lattice test/method_call5.cc.filter-bad.cc
#set args -q-config ../libqual/config/lattice test/taint1.c -q-serialize-qual test/taint1.c.1.qdir
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -q-serialize-qual test/taint1.c.2.qdir
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -q-serialize-qual test/taint1.c.2.qdir
#  file ./oink
#  set args test/double_goto.i
#set args -q-config ../libqual/config/lattice test/taint1.c -q-serialize-qual test/taint1.c.1.qdir
#set args -fq-link -q-config ../libqual/config/lattice test/taint1.c.1.qdir -q-serialize-qual test/taint1.c.2.qdir
#set args -fq-poly -q-config ../libqual/config/lattice test/taint2_a.c test/taint2_b.c.qdir
# set args -q-config ../libqual/config/lattice test/parse_mess1.cc
#set args -q-config ../libqual/config/lattice test/taint1.c.1.qdir -q-serialize-qual test/taint1.c.2.qdir
# set args -q-config ../libqual/config/lattice -tr nohashline xx.i
#  file ./staticprint
#  set args -fs-print-ihg test/hello.cc test/hello2.cc
#set args -q-config ../libqual/config/lattice test/taint1.c
#  file ./cc_qual
#  set args -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c.ctl
#file ./oink
#set args -fo-pretty-print ../elsa/in/t0543.cc
#set args ./oink -fo-pretty-print ../elsa/in/t0545.cc
#set args -fo-pretty-print ../elsa/in/d0027.cc
#  file ./qual
#  set args -fq-poly -o-lang kandrC -fq-print-quals-graph -q-config ../libqual/config/lattice Test/prelude_A0a.qdir Test/prelude_A0b.qdir

# file ./oink
#file ./qual
# set args ../elsa/in/t0001.cc
#set args -tr permissive -fo-no-do-elaboration -fo-print-ast-histogram imymoneystorageformat.cpp.d13830568afe72b84beaf3afdc5b0dd4.ii
#set args Test/return_struct.c
#set args /home/quarl/a/w.i -q-config ../libqual/config/lattice
# set args /home/quarl/a/t2.i -q-config ../libqual/config/lattice  -fo-instance-sensitive
#-o-lang GNU_Cplusplus
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/tertiary_cast_func_ptr.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/function_pointer_in_array.c.filter-bad.cc
# file ./dfgprint
# set args -fd-no-print-dfg Test/inline_asm1.cc
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/pass2.cc.filter-bad.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice foo1.ii
#set args  -fo-pretty-print ../elsa/in/t0545.cc
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/t1202ptrfuncall.c.filter-bad.cc
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/t1202ptrfuncall.c.filter-bad.cc
#set args Test/double_goto.i
#set args -fo-pretty-print ../elsa/in/t0001.cc
# file ./dfgprint
# set args -fd-print-dfg Test/dfgprint1.c
#file ./qual
# set args -tr xmlPrintAST-indent -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c.1.qdir -q-srz Test/taint1.c.2.qdir
# file ./dfgprint
# set args -fd-print-dfg Test/dfgprint1.c

# set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config ../libqual/config/lattice Test/void_star_poly1.cc.filter-good.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -o-control Test/prelude_A4b.ctl Test/prelude_A4a.qdir Test/prelude_A4b_nof.qdir

#file ./qual
# set args -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -tr nohashline -fq-no-explain-errors -q-config ../libqual/config/lattice Test/math.i -q-srz math3f.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c.1.qdir -q-srz Test/taint1.c.2.qdir
#set args -fq-no-explain-errors -fq-no-avoid-unused-decls -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -fq-no-avoid-unused-decls -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/unsat_symbol4.qdir

#set args -fq-no-explain-errors -fo-no-do-elaboration -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/struct_sizeof.c
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c.1.qdir -q-srz Test/taint1.c.2.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c.1.qdir
#set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c.1.qdir
#set args -tr xmlPrintAST-indent -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice Test/taint1.c -q-srz Test/taint1.c.1.qdir

# set args -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice ../elsa/in/big/nsCLiveconnectFactory.i -q-srz ../elsa/in/big/nsCLiveconnectFactory.i.1.qdir
# file oink
# set args -o-lang GNU_Cplusplus ../elsa/in/big/nsCLiveconnectFactory.i
#file qual
# set args -tr xmlPrintAST-indent -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-ret-inf -q-config ../libqual/config/lattice delta2/x.ii -q-srz delta2/x.ii.1.qdir
#set args -tr xmlPrintAST-indent -fq-no-explain-errors -fq-no-ret-inf -q-config ../libqual/config/lattice delta2/x.ii.1.qdir -q-srz delta2/x.ii.2.qdir
#file ../elsa/ccparse
#set args -tr parseXml,parseXml-no-danglingPointers,no-typecheck,no-elaborate -tr no-elaborate,xmlPrintAST,xmlPrintAST-types Test/inherit0.ii.1.xml
#set args -tr parseXml,parseXml-no-danglingPointers,no-typecheck,no-elaborate -tr prettyPrint Test/inherit0.ii.1.xml
#set args -tr parseXml,parseXml-no-danglingPointers,no-typecheck,no-elaborate -tr xmlPrintAST,xmlPrintAST-types Test/inherit0.ii.1.xml
# file ./qual
# set args -q-config ../libqual/config/lattice -fo-instance-sensitive Test/vararg_input0.c
# set args -fo-instance-sensitive -fq-casts-preserve -q-config ../libqual/config/lattice Test/vararg_input0.c
#set args -fq-casts-preserve -q-config ../libqual/config/lattice Test/vararg_input0.c

#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/ssh1.c
# set args -fq-casts-preserve -q-config ../libqual/config/lattice Test/vararg_input0.c
# set args -o-lang GNU2_KandR_C -q-config ../libqual/config/lattice Test/tertiary_cast_func_ptr.c
# set args -o-lang GNU2_KandR_C -q-config ../libqual/config/lattice Test/empty_statement_expr1.c
# set args -o-lang GNU2_KandR_C -q-config ../libqual/config/lattice Test/return_struct_fs.c
# set args -fq-no-explain-errors -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config ../libqual/config/lattice Test/return_struct_fs.c.filter-bad.cc

#set args -q-config ../libqual/config/lattice Test/transparent_union1.c
# set args -q-config ../libqual/config/lattice -fo-no-instance-sensitive Test/return_struct_fs.c

#set args -fq-no-explain-errors -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config ../libqual/config/lattice Test/transparent_union1.c.filter-bad.cc
# set args -fq-no-explain-errors -o-lang GNU2_KandR_C -q-config ../libqual/config/lattice Test/transparent_union1.c.filter-bad.cc

#set args -fo-instance-sensitive -q-config ../libqual/config/lattice Test/function_pointer_in_array.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -o-lang SUFFIX Test/main_externc1a.cc Test/main_externc1b.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/extern_inline_func2a.c -q-srz Test/extern_inline_func2a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2linked.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func3a-opt.qdir Test/extern_inline_func3b-opt.qdir -q-srz Test/extern_inline_func3linked-opt.qdir

# file ./oink
# set args  Test/atom_type_tag.cc
#file ./qual
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/method_call3g.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/vararg_input1.c.filter-bad.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/vararg_input1.c.filter-bad.cc
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/vararg_input1.c.filter-bad.cc
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice foo.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/vararg_input1.c.filter-bad.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/vararg_copy2.c.filter-bad.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/vararg_input1.c.filter-bad.cc
# set args -fo-instance-sensitive -fq-no-explain-errors -q-config ../libqual/config/lattice foo.c
#set args -fo-no-instance-sensitive -fq-no-explain-errors -q-config ../libqual/config/lattice foo.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/unprototyped1a.c Test/unprototyped1b.c -fo-report-unsatisfied-symbols
#set args -fq-name-with-serialno -fo-report-unsatisfied-symbols -fq-no-explain-errors -q-config ../libqual/config/lattice gronk.c gronkb.c
#set args -fq-no-explain-errors -fq-name-with-serialno -fq-casts-preserve -fq-name-with-loc -q-config ../libqual/config/lattice -fq-print-quals-graph -fq-ugly -fq-no-ret-inf  gronk.c gronkb.c
#set args -fq-no-explain-errors -fq-name-with-serialno -fq-casts-preserve -fq-name-with-loc -q-config ../libqual/config/lattice -fq-print-quals-graph -fq-ugly -fq-no-ret-inf  gronk.c gronkb.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/unprototyped1a.c Test/unprototyped1b.c -fo-report-unsatisfied-symbols

#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -q-srz Test/extern_inline_func2linked.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/ellipsis_as_void_star_poly5a.c -q-srz Test/ellipsis_as_void_star_poly5a.qdir

# file ./ccparse
# set args -tr parseXml,no-elaborate,prettyPrint  outdir/t0086.cc.B1.xml_filtered
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -q-srz Test/extern_inline_func2linked.qdir
#file Lib/union_find_test
# file ./oink
# set args Test/double_goto.i
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -q-srz Test/extern_inline_func2linked.qdir
# file ./qual
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_a.c -q-srz Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_b.c -q-srz Test/inst_sens_srz_b.qdir
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir

# VL55 the map entry is serialized but not the value
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_a.c -q-srz Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_a.c -q-srz Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_a.c -q-srz Test/inst_sens_srz_a.qdir
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir

#file ./qual
#set args -tr xmlPrintAST-indent -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_a.c -q-srz Test/inst_sens_srz_a.qdir
# ./qual -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_b.c -q-srz Test/inst_sens_srz_b.qdir
# ./qual -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir; test $? -eq 32
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_a.c -q-srz Test/inst_sens_srz_a.qdir
# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir
#file ./qual
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_b.qdir
# ./qual -tr xmlPrintAST-indent -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive Test/inst_sens_srz_b.c -q-srz Test/inst_sens_srz_b.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-instance-sensitive -fo-report-unsatisfied-symbols Test/inst_sens_srz_a.qdir Test/inst_sens_srz_b.qdir -q-srz Test/inst_sens_srz2.qdir

# # file ./oink
# file ./qual
# #set args -fo-report-unsatisfied-symbols foo.c
# set args -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols foo.c

# file ./qual
# # set args -q-config ../libqual/config/lattice -fo-pretty-print ../elsa/in/t0148.cc
# set args -q-config ../libqual/config/lattice -fo-pretty-print bar.cc
# file ./ccparse
# set args -tr parseXml,no-elaborate,prettyPrint  outdir/t0086.cc.B1.xml_filtered

#file ./qual
#set args ./qual  -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func5a.c Test/extern_inline_func5b.c
#set args ./qual  -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func5a.c

# file ./oink
# set args -fo-pretty-print ../elsa/in/d0097.cc
#file ./oink
# file ./qual
#set args -fo-report-unsatisfied-symbols foo.c
# set args -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols foo.c

# file ./oink
# set args -fo-pretty-print ../elsa/in/t0148.cc

# file ./oink
# # set args -q-config ../libqual/config/lattice -fo-pretty-print ../elsa/in/t0148.cc
# # set args -q-config ../libqual/config/lattice -fo-pretty-print bar.cc
# #set args bar.cc
# set args -fo-pretty-print ../elsa/in/t0369.cc

# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/unsat_symbol3b.qdir Test/unsat_symbol3c.qdir -o-control Test/unsat_symbol3c.ctl

# file ./qual
# set args -q-config ../libqual/config/lattice Test/weak1a.cc Test/weak1b.cc -fo-report-unsatisfied-symbols -o-control Test/weak1.ctl

# file ./oink
# set args /tmp/d.i

# file ./oink
# set args -tr nohashline -fo-report-unsatisfied-symbols a/a.c  a/error.oz -o-control a/GLIBC.oz.ctl

# file ./qual
# set args -fo-report-unsatisfied-symbols -o-lang SUFFIX -q-config /home/quarl/proj/debfsv/analyzer/lattice -fq-casts-preserve-below-functions -fq-poly -tr no_writeSerialNo -tr nohashline -o-control /home/quarl/proj/debfsv/glibc-fs/GLIBC.qz.ctl  /home/quarl/proj/debfsv/tests/dhcp/dhcp_2.0pl5-19.1/i/dhcp_2.0pl5-19.1/errwarn.c.bd7ba78e9a1a00d41f4a0f1db5544e80.i /home/quarl/proj/debfsv/glibc-fs/GLIBC.qz

# file ./oink
# set args a.i

# file ./qual
# set args  -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -q-srz Test/extern_inline_func2linked.qdir

# cd  /home/quarl/proj/debfsv/analyzer/oink
# file qual
# set args -q-config /tmp/lattice /home/quarl/proj/debfsv/analyzer/oink/Test/a.cc

# file ./qual

# set args -fq-no-explain-errors -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config ../libqual/config/lattice Test/void_star_poly1call.cc.filter-good.cc

# file ./oink
# #set args Test/atom_type_tag.cc
# set args -fo-report-unsatisfied-symbols Test/extern_undefined1.cc

# cd /net/boink.cs.berkeley.edu/srv/debsec/output/qual/i/libjpeg6b_6b-10
# file qual
# set args  -q-config /tmp/lattice -fq-poly a6-1.qdir -q-srz a6-2.qdir

# cd /net/boink.cs.berkeley.edu/srv/debsec/output/qual/i/libjpeg6b_6b-10
# file qual
# #set args -q-config /tmp/lattice  a7-1.qdir
# set args -q-config /tmp/lattice  a7.i -q-srz a7-1.qdir

# cd /home/quarl/proj/debfsv/analyzer/oink
# file qual
# set args -q-config /tmp/lattice Test/union_statement_expr1.c

# cd  /home/quarl/proj/debfsv/analyzer/oink
# file qual
# set args -q-config /tmp/lattice /home/quarl/proj/debfsv/analyzer/oink/Test/a.cc

# file ./qual
# set args -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/assert_type1.c
# set args -fq-no-explain-errors -o-lang GNU2_KandR_C -fo-instance-sensitive -q-config ../libqual/config/lattice Test/void_star_poly1call.cc.filter-good.cc

# file ./qual
# set args  -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2linked.qdir

# file ./oink
# set args Test/namespace_static1.cc

# file ./qual
# set args -q-config ../libqual/config/lattice -fo-instance-sensitive foo2.c

# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fq-casts-preserve-below-functions -fo-instance-sensitive Test/function_through_void1.c

# file qual
# set args  -q-config /tmp/lattice /home/quarl/proj/debfsv/analyzer/oink/Test/casts_preserve_func2.c -q-srz /tmp/a.qdir

# file qual
# set args -q-config /tmp/lattice /home/quarl/proj/debfsv/analyzer/oink/Test/unprototyped5a.c

# file qual
# set args  -q-config /tmp/lattice /tmp/a.c -q-srz /tmp/a.qdir

# file oink
# set args ./oink Test/implicit1.cc -o-srz Test/implicit1.odir

# file ./qual
# set args -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc

# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/template_class1.cc.filter-good.cc

# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -q-srz Test/extern_inline_func2linked.qdir

# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fq-casts-preserve-below-functions Test/casts_preserve_func2.c -q-srz Test/casts_preserve_func2.qdir

# file ./qual
# set args  -q-config ../libqual/config/lattice Test/allvalues.cc -q-srz Test/allvalues.cc.1.qdir

# file qual
# cd /home/quarl/proj/debfsv/analyzer/oink/pkg/bash3/bash3_3.0-12
# set args -fo-report-unsatisfied-symbols -o-lang SUFFIX -q-config /home/quarl/proj/debfsv/analyzer/lattice -fq-casts-preserve-below-functions -fq-poly -tr no_writeSerialNo -tr nohashline -fo-no-report-unsatisfied-symbols a.qz.tmp_pass1.qz -q-srz a.qz.tmp_pass2.qz

# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/unprototyped5a.c Test/unprototyped5b.c -fo-report-unsatisfied-symbols

# file  qual
# cd /home/quarl/proj/debfsv/analyzer/oink
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/void_nonfs1.cc

# file qual
# cd /home/quarl/proj/debfsv/analyzer/oink/pkg/bash3/bash3_3.0-12
# set args -fo-report-unsatisfied-symbols -o-lang SUFFIX -q-config /home/quarl/proj/debfsv/analyzer/lattice -fq-casts-preserve-below-functions -fq-poly -tr no_writeSerialNo -tr nohashline -fo-no-report-unsatisfied-symbols a.qz.tmp_pass1.qz -q-srz a.qz.tmp_pass2.qz

# file qual
# set args -q-config /tmp/lattice Test/unprototyped5a.c

# cd /home/quarl/proj/debfsv/analyzer/oink
# file ./qual
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/return_func1.1.qdir -q-srz Test/return_func1.2.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/return_func1.c -q-srz Test/return_func1.1.qdir
#set args -fq-no-explain-errors -fo-do-op-overload -q-config ../libqual/config/lattice Test/overload_oper2.cc
#set args -fq-poly -q-config ../libqual/config/lattice Test/poly_global2.cc.filter-good.cc
#set args -fq-no-explain-errors -fq-print-trans-qual -q-config ../libqual/config/lattice Test/func3.c
#set args -q-config /tmp/lattice
#set args -q-config /tmp/lattice Test/poly1.c
#set args -fq-no-explain-errors -fo-do-op-overload -q-config ../libqual/config/lattice Test/overload_oper2.cc
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/ptr-to-member2.cc.filter-good.cc
#set args -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc
#set args -fq-no-inference
#set args -q-config ../libqual/config/lattice Test/return_func1.c
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/unsat_symbol5b.qdir
#set args -q-config /tmp/lattice
# set args -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc 
# set args -q-config /tmp/lattice Test/unprototyped5a.c 
# file ./oink
#set args Test/template_class_member_srz1.cc -o-srz Test/template_class_member_srz1.odir
#set args -fq-no-explain-errors -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc
#set args -q-config /tmp/lattice Test/unprototyped5a.c
#set args -q-config /tmp/lattice  -fo-print-stages -fo-print-stats a.ii
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/extern_inline_func2b.c -q-srz Test/extern_inline_func2b.qdir
#set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/taint2_a.c Test/taint2_b.c
#set args -q-config ../libqual/config/lattice -fo-pretty-print -fq-print-trans-qual Test/taint1.c -fo-print-stages
#set args -fq-no-explain-errors -fq-poly -q-config ../libqual/config/lattice Test/poly_global2.cc.filter-bad.cc

# file ./qual
# set args -fq-no-explain-errors -o-lang GNU2_KandR_C -q-config ../libqual/config/lattice Test/char_array_lit_init.i

# file ./oink
#set args Test/template_class_member_srz1.cc -o-srz Test/template_class_member_srz1.odir
# set args Test/default_from_typename.cc
# file ./qual
# set args -q-config ../libqual/config/lattice Test/template_dep_resrz1.1.qdir -q-srz Test/template_dep_resrz1.2.qdir

# file ./oink
# set args Test/template_class_member_srz1.cc -o-srz Test/template_class_member_srz1.odir
# set args Test/implicit1.cc -o-srz Test/implicit1.odir
# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/template_class4_func_fwd1.cc.filter-good.cc
# file ./oink
# set args Test/template_default_nodef1.cc
# file ./qual
# set args -q-config ../libqual/config/lattice                                    Test/template_dep_resrz1.1.qdir -q-srz Test/template_dep_resrz1.2.qdir
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/unprototyped6.c
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/inconsistent_func_type_param1.c
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -fq-casts-preserve-below-functions Test/casts_preserve_func2.qdir
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -fq-casts-preserve-below-functions Test/casts_preserve_func2.qdir

# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -fo-report-unsatisfied-symbols Test/extern_inline_func2a.qdir Test/extern_inline_func2b.qdir -q-srz Test/extern_inline_func2linked.qdir

#set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/unprototyped5a.c Test/unprototyped5b.c -fo-report-unsatisfied-symbols -fo-instance-sensitive

# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/unprototyped5a.c Test/unprototyped5b.c -fo-report-unsatisfied-symbols -fo-instance-sensitive

# set args  -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/unprototyped6.c

# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -o-lang GNU2_KandR_C -fo-instance-sensitive Test/return_struct_fs.c.filter-good.cc
#set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/unprototyped4a.c Test/unprototyped4b.c -fo-report-unsatisfied-symbols
# set args -q-config ../libqual/config/lattice Test/template_dep_resrz1.1.qdir -q-srz Test/template_dep_resrz1.2.qdir

# file ./oink
# # set args Test/operator_bracket.ii Test/empty.i
# # set args Test/operator_bracket.ii
# set args Test/cast_array_init1.cc
# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/noparam_compound1.c
# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/noparam_compound1.c
# file ./oink
# set args Test/template_constr1.cc
#set args Test/oldstyle-too-many-args1.c
# set args Test/bin_minmax1.cc
# set args Test/nested_no_def_constr1.cc
# set args Test/default_from_typename.cc

# set args Test/const_array2.cc
# set args Test/cast_array_init1.cc
# file ./qual
# set args -fq-no-explain-errors -q-config ../libqual/config/lattice Test/noparam_compound1.c
# file ./oink
# set args Test/template_extern_srz1.cc -o-srz Test/template_extern_srz1.odir
# file ./oink
# set args ../elsa/in/t0117.cc
# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/ptr-to-member-cast1.cc
# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -fq-casts-preserve -fq-casts-preserve-below-functions Test/casts_preserve_func1.c
# file ./oink
# set args ../elsa/in/t0151.cc
#set args Test/inline1a.cc Test/inline1b.cc -fo-report-link-errors
# set args Test/atom_type_tag.cc
# file ./dfgprint
# set args -fd-print-dfg Test/dfgprint1.c

# FAILING
# file ./oink
# set args ../elsa/in/k0105.cc

# file ./qual
# # set args -fq-no-explain-errors -fq-no-names -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc

# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0161.cc -o-srz /dev/shm/oink.buildtmp/test/t0161.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# #set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0161.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0161.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# qual-check-resrz/../elsa/in/t0161.cc
# file ./qual
# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0161.cc -o-srz /dev/shm/oink.buildtmp/test/t0161.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
# qual: Assertion failed: serializeVar_Qual(var), file qual.cc line 140
#file ./qual
#set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/unprototyped4a.c Test/unprototyped4b.c -fo-report-unsatisfied-symbols
# file ./oink
# set args Test/atom_type_tag.cc
# set args Test/double_goto.i

# file ./qual
# set args -fq-no-explain-errors -fq-no-names -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc
# set args -fq-no-explain-errors -fq-no-names -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc 

# set args -fq-no-explain-errors -fq-no-names -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc > Test/parse_mess1.cc.out1

# file  ./oink
# set args -fo-report-link-errors Test/implicit1.odir Test/implicit2.odir

# file ./oink
# set args -fo-report-link-errors Test/implicit1.odir Test/implicit2.odir
# set args Test/typeof_undefined1.c -fo-report-link-errors
# set args ../elsa/in/k0105.cc
# set args foo.cc
# set args foo.cc
# set args -fo-report-link-errors Test/outlined_template_dtor1.cc
# set args -fo-report-link-errors foo.cc
# set args Test/double_goto.i
# file ./cpdinit_test
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit1A.c
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit1C.c
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit6B.c
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit2.c
# set args -o-lang ANSI_C99 LibCpdInit/Test/cil_init.c
# file ./oink
# set args Test/compoundinit13.c
# set args -fo-func-gran ../elsa/in/t0001.cc
# set args -fo-func-gran ../elsa/in/t0545.cc
# set args -fo-func-gran ../elsa/in/d0053.cc

# file ./cpdinit_test
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit1A.c
# file ./oink
# # set args -fo-func-gran ../elsa/in/d0053.cc
# set args -fo-func-gran Test/func_gran1.cc
# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/func_gran1q.cc
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/func_gran1q.cc
# file ./oink
# set args Test/implicit1.cc -o-srz Test/implicit1.odir
# set args -fo-report-link-errors Test/implicit1.odir Test/implicit2.odir
# set args Test/compoundinit12.c
# file ./cpdinit_test
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit1A.c
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit1C.c
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit2.c
# file ./oink
# set args Examples/struct1.cc
# file ./cpdinit_test
# set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit2.c
# file ./oink
# set args Test/func_gran1.cc -o-func-filter Test/func_gran1.cc.vars -o-srz Test/func_gran1.cc.odir

# file ./oink
# set args Test/func_gran3.cc -o-func-filter Test/func_gran3.cc.vars -o-srz Test/func_gran3.cc.odir

# file ./qual
# set args -fq-no-explain-errors -fq-no-names -o-lang GNU_Cplusplus -fq-no-inference -fo-pretty-print -fo-no-print-startstop Test/parse_mess1.cc

# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0001.cc -o-srz /dev/shm/oink.buildtmp/test/t0001.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# file ./dfgprint
# set args -fd-print-dfg Test/dfgprint1.c

# file ./qual
# set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0014.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0014.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
# set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0014.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0014.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
#set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0014.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0014.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0014.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0014.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

#set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0001.cc -o-srz /dev/shm/oink.buildtmp/test/t0001.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# file ./qual
# # set args /dev/shm/oink.buildtmp/test/t0014.cc.1.qdir -fq-no-inference
# set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0026.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0026.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
# file./oink
# set args Test/implicit1.cc -o-srz Test/implicit1.odir

# qual-check-resrz/../elsa/in/t0026.cc

# file ./qual
# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0026.cc -o-srz /dev/shm/oink.buildtmp/test/t0026.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# ./qual -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0026.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0026.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
# file ./qual
# set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0026.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0026.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

# qual: Assertion failed: arguments.isEmpty(), file ../elsa/template.cc line 401
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/func_gran1q.cc -o-func-filter Test/func_gran1q.cc.vars -o-srz Test/func_gran1q.cc.qdir
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/ellipsis_noparam1.c
# file ./oink
# set args Test/virtual4.cc
# set args Test/func_gran1.cc -o-func-filter Test/func_gran1.cc.vars -o-srz Test/func_gran1.cc.odir

# set args Test/atom_type_tag.cc
# set args -fo-report-link-errors Test/virtual6.cc
# file ./qual
# set args Test/usedindataflow1.cc -fq-no-inference -o-srz Test/usedindataflow1.qdir
# file ./oink
# file ../elsa/ccparse
# set args -tr treeCheck Test/template_inner_default1.cc
# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/noparam_cast1.c

# should find flow but does not
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/inconsistent_func_type_param2.c
# file ./oink
#file ../elsa/ccparse
# set args ../elsa/in/t0148.cc
# set args ../elsa/in/t0149.cc
# set args Test/const_array2.cc
# set args ../elsa/in/t0151.cc

# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -fq-casts-preserve -fq-casts-preserve-below-functions Test/casts_preserve_func1.c
# file ./oink
# set args Test/too_many_initializers1.c
# set args Test/template_inner_default1.cc
# set args -fo-report-link-errors link1.cc

# file ./qual
# set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice -fq-casts-preserve -fq-no-casts-preserve-below-functions Test/casts_preserve_func1b.c

# file ./oink
# set args Test/oldstyle-ellipsis1.c

# file ./qual
# set args -fq-no-explain-errors -fq-no-names /dev/shm/oink.buildtmp/test/t0561.cc.1.qdir -o-srz /dev/shm/oink.buildtmp/test/t0561.cc.2.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

#file ./qual
# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0544.cc -o-srz /dev/shm/oink.buildtmp/test/t0544.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph

#set args -fq-no-explain-errors -fq-no-names -q-config ../libqual/config/lattice Test/builtin2.cc
# file ./oink
# set args /home/dsw/lang_c/array1.c

# ./qual -fq-no-explain-errors -fq-no-names  ../elsa/in/t0005.cc -o-srz /dev/shm/oink.buildtmp/test/t0005.cc.1.qdir -tr nohashlines -q-config ../libqual/config/taint.lattice -fq-no-compactify-graph
# file ./qual
#set args -fq-no-explain-errors -fq-no-names  ../elsa/in/t0005.cc -tr nohashlines -q-config ../libqual/config/taint.lattice
# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0303.cc -tr nohashlines -q-config ../libqual/config/taint.lattice
# set args -fq-no-explain-errors -fq-no-names ../elsa/in/t0319.cc -tr nohashlines -q-config ../libqual/config/taint.lattice

#file ./oink
# set args Examples/taras1.ii

# file ./qual
# set args -fq-no-explain-errors -fq-no-names  -q-config ../libqual/config/lattice -q-func-param-ret-qual '$untainted' Test/func_param_ret1.c
# set args -fq-no-explain-errors -fq-no-names  -q-config ../libqual/config/lattice Test/builtin2.cc

# file ./oink
# set args -fo-report-link-errors Test/virtual7b.cc

# file ./qual
# set args -fq-no-explain-errors -fq-no-names  -fq-stackness -q-config ../libqual/config/stackness.lattice Test/stackness1.i -fo-pretty-print -fq-print-trans-qual -fo-verbose 

# file ./qual
# set args -fq-no-explain-errors -fq-no-names  -q-config ../libqual/config/lattice Test/const_when_param1.c

# file ./qual
#set args -fq-no-explain-errors -fq-no-names  -q-config ../libqual/config/lattice Test/func_gran1q.cc -o-func-filter Test/func_gran1q.cc.vars
# set args -fq-no-explain-errors -fq-no-names  -fq-no-inference -fq-exclude-global-js Test/exclude1.c
#set args -fq-no-explain-errors -fq-no-names  -fq-no-inference -q-reg-stack '$!regme*' Test/stkvarreg_good1.c
# set args -fq-no-explain-errors -fq-no-names  -fq-no-inference -q-reg-stack '$!regme*' Test/stkvarreg_good1.c
# set args -fq-no-explain-errors -fq-no-names  -fq-no-inference -q-reg-stack '$!regme*' Test/stkvarreg_bad5.c
# set args -fq-no-explain-errors -fq-no-names  -fq-no-inference -q-reg-stack '$!regme*' Test/stkvarreg_bad10.c
# file ./staticprint
# set args -fo-pretty-print Test/hello.cc
# file ./cpdinit_test
# file ./qual
# set args -o-module Test/foo_dupl
#set args -o-lang ANSI_C99 LibCpdInit/Test/compoundinit1A.c

# set args -fq-module-access -fq-poly -fo-no-instance-sensitive -fq-no-use-const-subtyping -fq-no-names -fq-no-explain-errors -fq-no-name-with-loc -q-config Test/mod_gronk_baz_new.lattice -o-mod-spec gronk:Test/mod_gronk.mod -o-mod-spec baz:Test/mod_baz.mod Test/mod_gronk_baz_new.ii 
#set args -fq-module-access -fq-poly -fo-no-instance-sensitive -fq-no-use-const-subtyping -q-config Test/mod_gronk_baz_method.lattice -o-mod-spec gronk:Test/mod_gronk.mod -o-mod-spec baz:Test/mod_baz.mod -o-mod-default default -fo-print-stages Test/mod_gronk_baz_method.ii

file ./alloctool
# set args -fa-print-stack-alloc-addr-taken Test/stack_alloc1.cc
# set args -fa-heapify-stack-alloc-addr-taken Test/heapify1.c
#set args -fa-heapify-stack-alloc-addr-taken Test/heapify2.c
# set args -fa-print-stack-alloc-addr-taken Test/stack_alloc_parse_qual.cc
# set args -fa-heapify-stack-alloc-addr-taken Test/heapify1.c
# set args -fa-heapify-stack-alloc-addr-taken Test/heapify1.c
# set args -fa-heapify-stack-alloc-addr-taken Test/heapify1.c

#set args -fa-verify-cross-module-params Test/verify1_foo.i Test/verify1_bar.i -o-mod-spec bar:Test/verify1_bar.mod -o-mod-spec foo:Test/verify1_foo.mod -o-mod-default default 
set args -fa-localize-heap-alloc Test/verify1_foo.i Test/verify1_bar.i -o-mod-spec bar:Test/verify1_bar.mod -o-mod-spec foo:Test/verify1_foo.mod -o-mod-default default 

break main
break breaker
break malloc_error_break

run
