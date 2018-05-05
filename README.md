# OS2018spring-projects-g08
http://os.cs.tsinghua.edu.cn/oscourse/OS2018spring/projects/g08


## Ucore for risc-v 64bit

### Run:

1. Configure:
``
  make menuconfig ARCH=riscv_64
``
or
``
  make ARCH=riscv_64 defconfig
``

2. Create file system
``
  make sfsimg
``

3. (Optional) create swap file
If you define "Support for paging of anonymous memory(swap)" in menuconfig, then run:
``
  make swapimg
``

4. Create kernel
``
  make
``

5. Run ucore
``
  ./uCore_run
``

## 学习笔记

 * LKM: https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/lkm.md
 * RISC-V pk/BBL: https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/pk_bbl.md
 * ucore+ i386, ucore diff: https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/ucore%2B%20i386%20vs%20ucore.md
 * ucore+ rv64 porting: https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/ucore%2B_rv64_porting.md
 * RISC-V ISA: https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/riscv_priv1.10.md, https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/riscv_spec2.2.md
 * SMP: https://github.com/riscv-labs/OS2018spring-projects-g08/blob/master/docs/smp.md

## 参考


 * RISC-V specifications (http://riscv.org)
    * User-level ISA specifications
    * Privileged ISA specifications
 * ucore for RISC-V 64 (https://gitee.com/shzhxh/ucore_os_lab)
 * ucore+ (https://github.com/chyyuu/ucore_os_plus)
 * bbl-ucore (https://ring00.github.io/bbl-ucore/)
 * RISC-V Proxy Kernel and Boot Loader (https://github.com/riscv/riscv-pk)
 * ucore SMP in x86 (http://os.cs.tsinghua.edu.cn/oscourse/OS2013/projects/U01)
 * ''The Scalable Commutativity Rule''
     * commuter (https://pdos.csail.mit.edu/archive/commuter/)
    * sv6 (https://github.com/aclements/sv6)
 * 蓝昶ucore+ LKM (http://os.cs.tsinghua.edu.cn/oscourse/OS2012/projects/U06)

