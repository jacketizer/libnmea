/*      $OpenBSD: memrchr.c,v 1.2 2007/11/27 16:22:12 martynas Exp $    */
/*
 * Copyright (c) 2007 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $FreeBSD: head/lib/libc/string/memrchr.c 178051 2008-04-10 00:12:44Z delphij $
 */


/*
 * Reverse memchr()
 * Find the last occurrence of 'c' in the buffer 's' of size 'n'.
 */
void *
memrchr(const void *s, int c, size_t n)
{
        const unsigned char *cp;

        if (n != 0) {
                cp = (unsigned char *)s + n;
                do {
                        if (*(--cp) == (unsigned char)c)
                                return((void *)cp);
                } while (--n != 0);
        }
        return(NULL);
}

void *
rawmemchr(const void *src, int c)
{
	#define SS (sizeof(size_t))
	#define ALIGN (sizeof(size_t)-1)
	#define ONES ((size_t)-1/UCHAR_MAX)
	#define HIGHS (ONES * (UCHAR_MAX/2+1))
	#define HASZERO(x) (((x)-ONES) & ~(x) & HIGHS)

	long n = LONG_MAX;
	const unsigned char *s = src;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & ALIGN) && n && *s != c; s++, n--);
	if (n && *s != c) {
		const size_t *w;
		size_t k = ONES * c;
		for (w = (const void *) s; n >= SS && !HASZERO(*w ^ k); w++, n -= SS);
		for (s = (const void *) w; n && *s != c; s++, n--);
	}
	return n ? (void *)s : 0;
}

