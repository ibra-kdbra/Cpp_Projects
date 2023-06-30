#include "../libft.h"

static int	is_flag(const char *s)
{
	char	*ids;

	ids = "cspdiuxX%";
	if (*s != '%')
		return (0);
	while (*ids)
	{
		if (*(s + 1) == *ids++)
			return (1);
	}
	return (0);
}

static int	fmt_id(char c, va_list fl, int nbc, int fd)
{
	if (c == 'd' || c == 'i')
		nbc = ft_putnbr_fd(va_arg(fl, int), nbc, fd);
	if (c == 'u')
		nbc = ft_putnbr_fd(va_arg(fl, unsigned), nbc, fd);
	if (c == 'p')
		nbc += ft_putptr_fd(va_arg(fl, unsigned long), fd);
	if (c == 'x' || c == 'X')
		nbc = ft_puthex_fd(va_arg(fl, unsigned), c == 'x', nbc, fd);
	if (c == 'c')
		nbc += ft_putchar_fd(va_arg(fl, int), fd);
	if (c == 's')
		nbc += ft_putstr_fd(va_arg(fl, char *), fd);
	if (c == '%')
		nbc += ft_putchar_fd('%', fd);
	return (nbc);
}

int	ft_dprintf(int fd, const char *fmt, ...)
{
	int		nbc;
	va_list	fl;

	nbc = 0;
	va_start(fl, fmt);
	while (*fmt)
	{
		if (is_flag(fmt))
		{
			nbc = fmt_id(*(fmt + 1), fl, nbc, fd);
			fmt++;
		}
		else
			nbc += ft_putchar_fd(*fmt, fd);
		fmt++;
	}
	va_end(fl);
	return (nbc);
}
